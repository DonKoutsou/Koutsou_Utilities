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
	
	[Attribute("5")]
	int m_CharacterTaskCompleteRepBonus;
	
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
			
			if (id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty))
			{
				FactionComp.SetAffiliatedFactionByKey("RENEGADE");
				if (EntityUtils.IsPlayer(Killer))
				{
					SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
				}
			}
			else
			{
				if (EntityUtils.IsPlayer(Killer))
				{
					SCR_HintManagerComponent.GetInstance().ShowCustom("Killed unit of your own faction, your reputation is worsened by alot");
				}
			}
			return;
		}
		if(instigator.IsFactionFriendly(Afflicted))
		{
			if (EntityUtils.IsPlayer(Killer))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("You caused issues between %1 and %2, your reputation has worsened", instigator.GetFactionKey(), Afflicted.GetFactionKey()));
			}
			if (id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty))
			{
				FactionComp.SetAffiliatedFactionByKey("RENEGADE");
				if (EntityUtils.IsPlayer(Killer))
				{
					SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
				}
			}
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
		FactionAffiliationComponent FactionCompAssignee = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent FactionCompOwner = FactionAffiliationComponent.Cast(TaskOwner.FindComponent(FactionAffiliationComponent));
		SCR_Faction instigatorFaction = SCR_Faction.Cast(FactionCompAssignee.GetAffiliatedFaction());
		SCR_Faction OwnerFaction = SCR_Faction.Cast(FactionCompOwner.GetAffiliatedFaction());
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Assignee.FindComponent(SCR_CharacterIdentityComponent));
		id.AdjustCharRep(m_CharacterTaskCompleteRepBonus);
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
		SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Factions %1 and %2 have called a ceasefire", factionA.GetFactionKey(), factionB.GetFactionKey()));
	}
	override void SetFactionsHostile(notnull SCR_Faction factionA, notnull SCR_Faction factionB, int playerChanged = -1)
	{
		super.SetFactionsHostile(factionA, factionB, playerChanged);
		SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Factions %1 and %2 declared war on each other", factionA.GetFactionKey(), factionB.GetFactionKey()));
	}
};
//------------------------------------------------------------------------------------------------------------//
enum EFactionRelationState
{
	ENEMY,
	OKWITH,
	FRIENDLY
}
