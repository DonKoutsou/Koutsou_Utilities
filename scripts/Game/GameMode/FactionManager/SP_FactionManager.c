class SP_FactionManagerClass : SCR_FactionManagerClass
{
};
class SP_FactionManager : SCR_FactionManager
{
	[Attribute("5", desc: "applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_FactionFriendlyKillRepPenalty;
	
	[Attribute("20", desc: "applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_CharacterFriendlyKillRepPenalty;
	
	[Attribute("5")]
	int m_FactionTaskCompleteRepBonus;
	
	
	private SP_GameMode m_GameMode;

	EFactionRelationState GetFactionRelationState(SCR_Faction Faction1, SCR_Faction Faction2)
	{
		int relation1 = Faction1.GetFactionRep(Faction2);
		int relation2 = Faction2.GetFactionRep(Faction1);
		if (relation1 > -50 && relation2 > -50)
		{
			if (relation1 > 50 && relation2 > 50)
			{
				return EFactionRelationState.FRIENDLY;
			}
			return EFactionRelationState.OKWITH;
		}
		return EFactionRelationState.ENEMY;
	};
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
			
			id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty, Killer, "You killed a unit of you own faction.");
			return;
		}
		if(instigator.IsFactionFriendly(Afflicted))
		{
			id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty, Killer,string.Format("You caused issues between %1 and %2 forces.", instigator.GetFactionKey(), Afflicted.GetFactionKey()));
			Afflicted.AdjustRelation(instigator, -m_FactionFriendlyKillRepPenalty);
			array <Faction> friendlyfacts = new array <Faction>();
			Afflicted.GetFriendlyFactions2(friendlyfacts);
			foreach(Faction frfact : friendlyfacts)
			{
				SCR_Faction scrfact = SCR_Faction.Cast(frfact);
				scrfact.AdjustRelation(instigator, m_FactionTaskCompleteRepBonus/2);
			}
			return;
		}
		
	}

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
			id.AdjustCharRep(task.GetRepReward(), Assignee, string.Format("You completed a task for %1.", diag.GetCharacterName(TaskOwner)));
		else
			id.AdjustCharRep(task.GetRepReward(), Assignee, string.Format("You completed a task for %1. The %2 forces will remember that. Your factions relations have improved", diag.GetCharacterName(TaskOwner), OwnerFaction.GetFactionKey()));
		OwnerFaction.AdjustRelation(instigatorFaction, m_FactionTaskCompleteRepBonus);
		array <Faction> friendlyfacts = new array <Faction>();
		OwnerFaction.GetFriendlyFactions2(friendlyfacts);
		foreach(Faction frfact : friendlyfacts)
		{
			SCR_Faction scrfact = SCR_Faction.Cast(frfact);
			scrfact.AdjustRelation(instigatorFaction, m_FactionTaskCompleteRepBonus/2);
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
};
//------------------------------------------------------------------------------------------------------------//
enum EFactionRelationState
{
	ENEMY,
	OKWITH,
	FRIENDLY
}
