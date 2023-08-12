class SP_FactionManagerClass : SCR_FactionManagerClass
{
};
class SP_FactionManager : SCR_FactionManager
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
	
	private SP_GameMode m_GameMode;

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
		if(instigator == Afflicted)
		{
			instigator.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty);
			id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty, Killer, "You killed a unit of you own faction.");
			return;
		}
		else if(Afflicted.IsFactionFriendly(instigator))
		{
			id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty, Killer,string.Format("You caused issues between %1 and %2 forces.", instigator.GetFactionKey(), Afflicted.GetFactionKey()));
			Afflicted.AdjustRelation(instigator, -m_FactionFriendlyKillRelationPenalty);
			instigator.AdjustRelation(Afflicted, -m_FactionFriendlyKillRelationPenalty);
			Afflicted.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty);
			//instigator.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty);
			array <Faction> friendlyfacts = new array <Faction>();
			Afflicted.GetFriendlyFactions2(friendlyfacts);
			foreach(Faction frfact : friendlyfacts)
			{
				if (frfact == instigator || frfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(frfact);
				scrfact.AdjustRelation(instigator, -m_FactionFriendlyKillRelationPenalty/2);
				instigator.AdjustRelation(scrfact, -m_FactionFriendlyKillRelationPenalty/2);
				scrfact.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty/2);
			}
			array <Faction> enemyfacts = new array <Faction>();
			Afflicted.GetEnemyFactions(enemyfacts);
			foreach(Faction enfact : enemyfacts)
			{
				if (enfact.GetFactionKey() == "RENEGADE")
					continue;
				if (enfact == instigator || enfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(enfact);
				scrfact.AdjustRelation(instigator, m_FactionFriendlyKillRelationImprovement);
				instigator.AdjustRelation(scrfact, m_FactionFriendlyKillRelationImprovement);
				scrfact.AdjustPlayerGoodwill(m_FactionFriendlyKillGoodwillImprovement);
			}
		}
		else if(Afflicted.IsFactionEnemy(instigator))
		{
			array <Faction> friendlyfacts = new array <Faction>();
			Afflicted.GetFriendlyFactions2(friendlyfacts);
			foreach(Faction frfact : friendlyfacts)
			{
				if (frfact == instigator || frfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(frfact);
				scrfact.AdjustRelation(instigator, -m_FactionFriendlyKillRelationPenalty/2);
				instigator.AdjustRelation(scrfact, -m_FactionFriendlyKillRelationPenalty/2);
				scrfact.AdjustPlayerGoodwill(-m_FactionFriendlyKillGoodwillPenalty/2);
			}
			array <Faction> enemyfacts = new array <Faction>();
			Afflicted.GetEnemyFactions(enemyfacts);
			foreach(Faction enfact : enemyfacts)
			{
				if (enfact.GetFactionKey() == "RENEGADE")
					continue;
				if (enfact == instigator || enfact == Afflicted)
					continue;
				SCR_Faction scrfact = SCR_Faction.Cast(enfact);
				scrfact.AdjustRelation(instigator, m_FactionFriendlyKillRelationImprovement);
				instigator.AdjustRelation(scrfact, m_FactionFriendlyKillRelationImprovement);
				scrfact.AdjustPlayerGoodwill(m_FactionFriendlyKillGoodwillImprovement);
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
		if (instigatorFaction == OwnerFaction)
		{
			OwnerFaction.AdjustPlayerGoodwill(m_TaskCompleteGoodwillBonus);
			id.AdjustCharRep(task.GetRepReward(), Assignee, string.Format("You completed a task for %1.", diag.GetCharacterName(TaskOwner)));
		}
		else
		{
			OwnerFaction.AdjustRelation(instigatorFaction, m_TaskCompleteFactionRelationBonus);
			instigatorFaction.AdjustRelation(OwnerFaction, m_TaskCompleteFactionRelationBonus);
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
			m_GameMode = SP_GameMode.Cast(GetGame().GetGameMode());
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
