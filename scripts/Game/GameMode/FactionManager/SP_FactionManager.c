modded class SCR_FactionManager
{
	[Attribute("250", desc: "Penalty to relation of 2 factions when killing friendly character. applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_FactionFriendlyKillRelationPenalty;
	
	[Attribute("250", desc: "Penalty to goodwill of player when killing friendly character. applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_FactionFriendlyKillGoodwillPenalty;
	
	[Attribute("50", desc: "Relation improvement when a faciton kills an enemy of another faction")]
	int m_FactionFriendlyKillRelationImprovement;
	
	[Attribute("50", desc: "Goodwill improvement when a player kills an enemy of another faction")]
	int m_FactionFriendlyKillGoodwillImprovement;
	
	[Attribute("5", desc: "Penalty to characters reputation when killing friendly, applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_CharacterFriendlyKillRepPenalty;
	
	[Attribute("15", desc: "Bonus to goodwill of player when completing task.")]
	int m_TaskCompleteGoodwillBonus;
	
	[Attribute("15", desc: "Bonus to relation of 2 factions when task of friendly faction is completed.")]
	int m_TaskCompleteFactionRelationBonus;
	
	private SCR_GameModeCampaign m_GameMode;

	EFactionRelationState GetFactionRelationState(SCR_Faction Faction1, SCR_Faction Faction2)
	{
		int relation1 = Faction1.GetFactionRep(Faction2);
		int relation2 = Faction2.GetFactionRep(Faction1);
		if (relation1 > -500 && relation2 > -500)
		{
			if (relation1 > 500 && relation2 > 500)
			{
				return EFactionRelationState.FRIENDLY;
			}
			return EFactionRelationState.OKWITH;
		}
		return EFactionRelationState.ENEMY;
	};
	//Handles death and checks if penalties need to be applied to reputations and faction relations.
	//If there was kill of friendly faction takes away goodwill and reputation of killer, and degrades relation of 2 factions
	void HandleDeath(IEntity Victim, IEntity Killer)
	{
		if(!Killer)
			return;
		FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(Killer.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent FactionCompVictim = FactionAffiliationComponent.Cast(Victim.FindComponent(FactionAffiliationComponent));
		SCR_Faction instigator = SCR_Faction.Cast(FactionComp.GetAffiliatedFaction());
		SCR_Faction Afflicted = SCR_Faction.Cast(FactionCompVictim.GetAffiliatedFaction());
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Killer.FindComponent(SCR_CharacterIdentityComponent));
		//If kiled unit of same faction
		if(instigator == Afflicted)
		{
			SCR_CharacterRankComponent rankComp = SCR_CharacterRankComponent.Cast(Victim.FindComponent(SCR_CharacterRankComponent));
			int multiplier = rankComp.GetCharacterRank(Victim);
			if (Killer == GetGame().GetPlayerController().GetControlledEntity() && SCR_EntityHelper.GetPlayer() == Killer)
				instigator.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty * multiplier);
			if (multiplier > 2)
				id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty * multiplier, Killer, "You killed a high ranking unit of you own faction.");
			else
				id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty * multiplier, Killer, "You killed a unit of you own faction.");
			return;
		}
		//If killed unit of friendly faction
		else if(Afflicted.IsFactionFriendly(instigator))
		{
			SCR_CharacterRankComponent rankComp = SCR_CharacterRankComponent.Cast(Victim.FindComponent(SCR_CharacterRankComponent));
			int multiplier = rankComp.GetCharacterRank(Victim);
			id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty * multiplier, Killer,string.Format("You caused issues between %1 and %2 forces.", instigator.GetFactionKey(), Afflicted.GetFactionKey()));
			//adjust relation between 2 factions
			Afflicted.AdjustRelation(instigator, -m_FactionFriendlyKillRelationPenalty * multiplier);
			instigator.AdjustRelation(Afflicted, -m_FactionFriendlyKillRelationPenalty * multiplier);
			//Adjust player goodwill, losed half from players fation
			if (Killer == GetGame().GetPlayerController().GetControlledEntity() && SCR_EntityHelper.GetPlayer() == Killer)
			{
				Afflicted.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty * multiplier);
				instigator.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty/2 * multiplier);
			}
			//Adjust relation of factions frenly to the afflicted.
			array <Faction> friendlyfacts = new array <Faction>();
			Afflicted.GetFriendlyFactions2(friendlyfacts);
			foreach(Faction frfact : friendlyfacts)
			{
				if (frfact == instigator || frfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(frfact);
				scrfact.AdjustRelation(instigator, -m_FactionFriendlyKillRelationPenalty/2 * multiplier);
				instigator.AdjustRelation(scrfact, -m_FactionFriendlyKillRelationPenalty/2 * multiplier);
				if (Killer == GetGame().GetPlayerController().GetControlledEntity() && SCR_EntityHelper.GetPlayer() == Killer)
					scrfact.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty/2 * multiplier);
			}
			//Do same for enemy factions
			array <Faction> enemyfacts = new array <Faction>();
			Afflicted.GetEnemyFactions(enemyfacts);
			foreach(Faction enfact : enemyfacts)
			{
				if (enfact.GetFactionKey() == "RENEGADE")
					continue;
				if (enfact == instigator || enfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(enfact);
				scrfact.AdjustRelation(instigator, m_FactionFriendlyKillRelationImprovement * multiplier);
				instigator.AdjustRelation(scrfact, m_FactionFriendlyKillRelationImprovement * multiplier);
				if (Killer == GetGame().GetPlayerController().GetControlledEntity() && SCR_EntityHelper.GetPlayer() == Killer)
					scrfact.AdjustPlayerGoodwill(m_FactionFriendlyKillGoodwillImprovement * multiplier);
			}
		}
		//if killed unit is of enemy faction
		else if(Afflicted.IsFactionEnemy(instigator))
		{
			SCR_CharacterRankComponent rankComp = SCR_CharacterRankComponent.Cast(Victim.FindComponent(SCR_CharacterRankComponent));
			int multiplier = rankComp.GetCharacterRank(Victim);
			//adjust relation of factions frendly to the afflicted
			array <Faction> friendlyfacts = new array <Faction>();
			Afflicted.GetFriendlyFactions2(friendlyfacts);
			foreach(Faction frfact : friendlyfacts)
			{
				if (frfact == instigator || frfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(frfact);
				scrfact.AdjustRelation(instigator, -m_FactionFriendlyKillRelationPenalty/2 * multiplier);
				instigator.AdjustRelation(scrfact, -m_FactionFriendlyKillRelationPenalty/2 * multiplier);
				if (Killer == GetGame().GetPlayerController().GetControlledEntity() && SCR_EntityHelper.GetPlayer() == Killer)
					scrfact.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty/2 * multiplier);
			}
			//adjust relation of faction enemy to the afflicted
			array <Faction> enemyfacts = new array <Faction>();
			Afflicted.GetEnemyFactions(enemyfacts);
			foreach(Faction enfact : enemyfacts)
			{
				if (enfact.GetFactionKey() == "RENEGADE")
					continue;
				if (enfact == instigator || enfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(enfact);
				scrfact.AdjustRelation(instigator, m_FactionFriendlyKillRelationImprovement * multiplier);
				instigator.AdjustRelation(scrfact, m_FactionFriendlyKillRelationImprovement * multiplier);
				if (Killer == GetGame().GetPlayerController().GetControlledEntity() && SCR_EntityHelper.GetPlayer() == Killer)
					scrfact.AdjustPlayerGoodwill(m_FactionFriendlyKillGoodwillImprovement * multiplier);
			}
		};
		
		return;
	}
	//Handles task completion and checks if penalties need to be applied to reputations and faction relations.
	//If completed tasks of friendly faction gives you goodwill for them and increases the relations of the 2 factions.
	void OnTaskCompleted(SP_Task task, IEntity Assignee)
	{
		IEntity TaskOwner = task.GetOwner();
		if (!TaskOwner)
			TaskOwner = task.GetTarget();
		if(task.GetOwner() == Assignee)
		{
			return;
		}
		SP_DialogueComponent diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		FactionAffiliationComponent FactionCompAssignee = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent FactionCompOwner = FactionAffiliationComponent.Cast(TaskOwner.FindComponent(FactionAffiliationComponent));
		SCR_Faction instigatorFaction = SCR_Faction.Cast(FactionCompAssignee.GetAffiliatedFaction());
		SCR_Faction OwnerFaction = SCR_Faction.Cast(FactionCompOwner.GetAffiliatedFaction());
		SCR_CharacterIdentityComponent Charid = SCR_CharacterIdentityComponent.Cast(TaskOwner.FindComponent(SCR_CharacterIdentityComponent));
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Assignee.FindComponent(SCR_CharacterIdentityComponent));
		//if completed task for my faction
		if (instigatorFaction == OwnerFaction)
		{
			if (SCR_EntityHelper.GetPlayer() == Assignee)
				OwnerFaction.AdjustPlayerGoodwill(m_TaskCompleteGoodwillBonus);
			id.AdjustCharRep(task.GetRepReward(), Assignee, string.Format("You completed a task for %1.", diag.GetCharacterName(TaskOwner)));
		}
		//if completed taskf ro friendly faction
		else
		{
			OwnerFaction.AdjustRelation(instigatorFaction, m_TaskCompleteFactionRelationBonus);
			instigatorFaction.AdjustRelation(OwnerFaction, m_TaskCompleteFactionRelationBonus);
			if (SCR_EntityHelper.GetPlayer() == Assignee)
				OwnerFaction.AdjustPlayerGoodwill(m_TaskCompleteGoodwillBonus);
			id.AdjustCharRep(task.GetRepReward(), Assignee, string.Format("You completed a task for %1. The %2 forces will remember that. Your factions relations have improved", diag.GetCharacterName(TaskOwner), OwnerFaction.GetFactionKey()));
			array <Faction> friendlyfacts = new array <Faction>();
			OwnerFaction.GetFriendlyFactions2(friendlyfacts);
			foreach(Faction frfact : friendlyfacts)
			{
				if (frfact == instigatorFaction || frfact == OwnerFaction)
						continue;
				SCR_Faction scrfact = SCR_Faction.Cast(frfact);
				scrfact.AdjustRelation(instigatorFaction, m_TaskCompleteFactionRelationBonus/2);
				instigatorFaction.AdjustRelation(scrfact, m_TaskCompleteFactionRelationBonus/2);
				scrfact.AdjustPlayerGoodwill(m_TaskCompleteGoodwillBonus/2);
			}
		}
	};

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!m_GameMode)
			m_GameMode = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		if (!m_GameMode)
			return;
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if (requestman)
			requestman.OnTaskComplete().Insert(OnTaskCompleted);
		m_GameMode.GetOnControllableDestroyed().Insert(HandleDeath);
		if(m_SortedFactions)
		{
			array<Faction> m_Factions = new array<Faction>();
			GetFactionsList(m_Factions);
			foreach (Faction fact : m_Factions)
			{
				SCR_Faction scrfact = SCR_Faction.Cast(fact);
				scrfact.OnRelationDropped().Insert(SetFactionsHostile);
				scrfact.OnRelationRaised().Insert(SetFactionsFriendly);
			}
		}
	};
	override void SetFactionsFriendly(notnull SCR_Faction factionA, notnull SCR_Faction factionB, int playerChanged = -1)
	{
		super.SetFactionsFriendly(factionA, factionB, playerChanged);
		if (factionA != factionB)
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Factions %1 and %2 have called a ceasefire", factionA.GetFactionKey(), factionB.GetFactionKey()));
	}
	override void SetFactionsHostile(notnull SCR_Faction factionA, notnull SCR_Faction factionB, int playerChanged = -1)
	{
		super.SetFactionsHostile(factionA, factionB, playerChanged);
		if (factionA != factionB)
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Factions %1 and %2 declared war on each other", factionA.GetFactionKey(), factionB.GetFactionKey()));
	}
	void GetEnemyFactions(Faction fact, out array <Faction> enemies)
	{
		array <Faction> factions = new array <Faction>();
		GetFactionsList(factions);
		foreach (Faction faction : factions)
		{
			if (fact.IsFactionEnemy(faction))
				enemies.Insert(faction);
		}
	}
	void GetFriendlyFactions(Faction fact, out array <Faction> friends)
	{
		array <Faction> factions = new array <Faction>();
		GetFactionsList(factions);
		foreach (Faction faction : factions)
		{
			if (fact.IsFactionFriendly(faction))
				friends.Insert(faction);
		}
	}
	void SetUpPlayerGoodwill(IEntity Player)
	{
		array <Faction> factions = new array <Faction>();
		GetFactionsList(factions);
		FactionAffiliationComponent fact = FactionAffiliationComponent.Cast(Player.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfaction = SCR_Faction.Cast(fact.GetAffiliatedFaction());
		foreach (Faction faction : factions)
		{
			SCR_Faction scrfact = SCR_Faction.Cast(faction);
			if (scrfact.m_iStartingPlayerGoodwill != -1 )
			{
				scrfact.SetPlayerGoodwill(scrfact.m_iStartingPlayerGoodwill);
			}
			if (scrfact == myfaction)
			{
				scrfact.SetPlayerGoodwill(1000);
			}
			else if (scrfact.IsFactionFriendly(myfaction))
			{
				scrfact.SetPlayerGoodwill(500);
			}
			else if (scrfact.IsFactionEnemy(myfaction))
			{
				scrfact.SetPlayerGoodwill(-500);
			}
		}
	}
};
//------------------------------------------------------------------------------------------------------------//
enum EFactionRelationState
{
	ENEMY,
	OKWITH,
	FRIENDLY
}
