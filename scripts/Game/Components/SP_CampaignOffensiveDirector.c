class SP_CampaignOffensiveDirectorClass : ScriptComponentClass
{
	
}
class SP_CampaignOffensiveDirector : ScriptComponent
{
	[Attribute("1000", desc : "How many atacks will happen durring a day")]
	int m_iAtackPer;
	
	[Attribute(desc: "Default waypoint if any WP group is defined", defvalue: "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et", category: "Waypoints")]
	protected ResourceName 						m_sAtackWP;
	
	[Attribute(desc: "Default waypoint if any WP group is defined", defvalue:"{B3E7B8DC2BAB8ACC}Prefabs/AI/Waypoints/AIWaypoint_SearchAndDestroy.et", category: "Waypoints")]
	protected ResourceName 						m_sRegroupWP;
	
	int factioncount;
	protected ref array<SCR_CampaignFaction> m_aFactions = {};
	protected ref array <ref OrganisedAtack> m_aOrganisedAtacks = {};
	AtackPoint m_APoint;
	float m_fTimePassed;
	//------------------------------------------------------------------------------------------------------------//
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	bool OrganiseAtack()
	{
		//pick a faction to atack randomly
		int index = Math.RandomInt(0, factioncount);
		SCR_CampaignFaction FactionToAtack = m_aFactions.Get(index);
		
		//Get their primary target
		SCR_CampaignMilitaryBaseComponent BaseToAtack = FactionToAtack.GetPrimaryTarget();
		if (!BaseToAtack)
			return false;
		
		foreach (OrganisedAtack AtackToCheck : m_aOrganisedAtacks)
		{
			if (AtackToCheck.BaseToAtack == BaseToAtack)
				return false;
		}
		vector BaseToAtackLocation = BaseToAtack.GetOwner().GetOrigin();
		
		//Get the faction that is going to be atacked
		SCR_CampaignFaction FactionToBeAtacked = SCR_CampaignFaction.Cast(BaseToAtack.GetFaction());
		
		//Get bases wich are in range to figure out where the troops for the atack will come from
		//we look for a base with same faction as atacker and get the group that is going to perform the atack
		array <SCR_CampaignMilitaryBaseComponent> basesinrange = {};
		BaseToAtack.GetBasesInRange(SCR_ECampaignBaseType.BASE, basesinrange);
		
		SCR_CampaignMilitaryBaseComponent AtackerBase;
		
		SCR_AIGroup GroupToAtack;
		
		foreach (SCR_CampaignMilitaryBaseComponent Base : basesinrange)
		{
			if (Base.GetFaction() != FactionToAtack)
				continue;
			GroupToAtack = Base.GetDefendersGroup();
			if (!GroupToAtack)
			{
				SCR_ServicePointComponent Baracks = Base.GetServiceByType(SCR_EServicePointType.BARRACKS);
				if (!Baracks)
					continue;
				SCR_DefenderSpawnerComponent DeffenderSpawner = SCR_DefenderSpawnerComponent.Cast(Baracks);
				if (!DeffenderSpawner)
					continue;
				Base.SetDefendersGroup( DeffenderSpawner.SpawnGroupExtrarnal());
				
				GroupToAtack = Base.GetDefendersGroup();
			}
			if (!GroupToAtack)
			{
				continue;
			}
			
			AtackerBase = Base;
			break;
		}
		if (!AtackerBase || !GroupToAtack)
			return false;
		//find an atacking point close to that base
		
		GetGame().GetWorld().QueryEntitiesBySphere(BaseToAtackLocation, 500, QueryforAtackPoint);
		
		if (!m_APoint)
			return false;
		
		//Spawn waypoints
		//Regroup WP
		EntitySpawnParams RegroupspawnParams	= new EntitySpawnParams();
		RegroupspawnParams.TransformMode		= ETransformMode.WORLD;
		RegroupspawnParams.Transform[3] = m_APoint.GetOrigin();
		Resource wpRes = Resource.Load(m_sRegroupWP);
		AIWaypoint RegroupWP = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(wpRes, GetGame().GetWorld(), RegroupspawnParams));
		
		//AtackWP
		EntitySpawnParams AtackspawnParams	= new EntitySpawnParams();
		AtackspawnParams.TransformMode		= ETransformMode.WORLD;
		AtackspawnParams.Transform[3] = BaseToAtackLocation;
		Resource wpARes = Resource.Load(m_sAtackWP);
		AIWaypoint AtackWP = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(wpARes, GetGame().GetWorld(), AtackspawnParams));
		
		//assign regroup waypoint
		GroupToAtack.AddWaypointAt(RegroupWP, 0);
		
		TaskDayTimeInfo AtackDateInfo = TaskDayTimeInfo.FromPointInFuture(8);
		
		OrganisedAtack Atack = new OrganisedAtack();
		Atack.TimeInfo = AtackDateInfo;
		Atack.GroupToAtack = GroupToAtack;
		Atack.AtackerFaction = FactionToAtack;
		Atack.DeffenderFaction = FactionToBeAtacked;
		Atack.AtackerBase = AtackerBase;
		Atack.BaseToAtack = BaseToAtack;
		Atack.AtackWP = AtackWP;
		Atack.RegroupWP = RegroupWP;
		m_aOrganisedAtacks.Insert(Atack);
		return true;
	}
	
	void CommenceAtack(OrganisedAtack Atack)
	{
		SCR_AIGroup Group =  Atack.GroupToAtack;
		if (!Group)
		{
			m_aOrganisedAtacks.RemoveItem(Atack);
			return;
		}
		Group.CompleteWaypoint(Atack.RegroupWP);
		Group.AddWaypointAt(Atack.AtackWP, 0);
	}
	
	void CheckForAnTack()
	{
		if (m_aOrganisedAtacks.IsEmpty())
			return;
		foreach (OrganisedAtack Atack : m_aOrganisedAtacks)
		{
			if (Atack.IsItTime())
				CommenceAtack(Atack);
		}
	}
	
	override void EOnInit(IEntity owner)
	{
		SCR_GameModeCampaign Gamemode = SCR_GameModeCampaign.Cast(owner);
		if (!Gamemode)
			return;
		SCR_CampaignFactionManager FactionMan = SCR_CampaignFactionManager.Cast(Gamemode.GetFactionManager());
		if (!FactionMan)
		{
			GetGame().GetCallqueue().CallLater(EOnInit, 1000, false, owner);
			return;
		}
		
		
		array<Faction> Factions = {};
		FactionMan.GetFactionsList(Factions);
		foreach (Faction faction : Factions)
		{
			SCR_CampaignFaction CFaction = SCR_CampaignFaction.Cast(faction);
			if (CFaction.IsPlayable())
			{
				m_aFactions.Insert(CFaction);
			}
		}
		factioncount = m_aFactions.Count();
		SetEventMask(owner, EntityEvent.FRAME);
	}
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_fTimePassed > 0)
		{
			CheckForAnTack();
			m_fTimePassed -= timeSlice;
			return;
		}
		if (OrganiseAtack())
			m_fTimePassed = m_iAtackPer;
		
		
	}
	private bool QueryforAtackPoint(IEntity e)
	{
		AtackPoint point = AtackPoint.Cast(e);
		if (!point)
			return true;
		m_APoint = point;
		return false;
	}
}
class OrganisedAtack
{
	SCR_CampaignFaction AtackerFaction;
	SCR_CampaignFaction DeffenderFaction;
	SCR_CampaignMilitaryBaseComponent AtackerBase;
	SCR_CampaignMilitaryBaseComponent BaseToAtack;
	SCR_AIGroup GroupToAtack;
	AIWaypoint AtackWP;
	AIWaypoint RegroupWP;
	
	ref TaskDayTimeInfo TimeInfo;
	
	bool IsItTime()
	{
		TaskDayTimeInfo now = TaskDayTimeInfo.FromTimeOfTheDay();
		return now.HasPassed(TimeInfo);
	}
	void ~OrganisedAtack ()
	{
		if (AtackWP)
			delete AtackWP;
		if (RegroupWP)
			delete RegroupWP;
	}
}
class AtackPointClass : GenericEntityClass
{
}
class AtackPoint : GenericEntity
{
}