[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class SCR_ScenarioFrameworkSlotAIClass : SCR_ScenarioFrameworkSlotBaseClass
{
	// prefab properties here
};

//------------------------------------------------------------------------------------------------
class SCR_ScenarioFrameworkSlotAI : SCR_ScenarioFrameworkSlotBase
{
	[Attribute(desc: "Waypoint Groups if applicable", category: "Waypoints")]
	protected ref array<ref SCR_WaypointSet> 	m_aWaypointGroupNames;

	[Attribute(desc: "Spawn AI on the first WP Slot", defvalue: "1", category: "Waypoints")]
	protected bool								m_bSpawnAIOnWPPos;

	[Attribute(desc: "Default waypoint if any WP group is defined", "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", category: "Waypoints")]
	protected ResourceName 						m_sWPToSpawn;

	[Attribute(defvalue: "{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et", category: "Waypoints")]
	protected ResourceName 										m_sGroupPrefab;

	[Attribute(defvalue: "{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et", category: "Waypoints")]
	protected ResourceName 										m_sCycleWPPrefab;

	[Attribute(desc: "Balancing group sizes based on number of players", defvalue: "0", category: "Misc")]
	protected bool								m_bBalanceOnPlayersCount;

	protected ref array<AIWaypoint> 			m_aWaypoints = {};
	protected SCR_AIGroup						m_AIGroup;
	protected ref array<ResourceName>			m_aAIPrefabsForRemoval = {};
	protected ref array<ref EntitySpawnParams> aPosOut = {};
	SCR_ScenarioFrameworkArea myarea;
	//------------------------------------------------------------------------------------------------
	override void Init(SCR_ScenarioFrameworkArea area = null, SCR_ScenarioFrameworkEActivationType activation = SCR_ScenarioFrameworkEActivationType.SAME_AS_PARENT, bool bInit = true)
	{
		if (activation == SCR_ScenarioFrameworkEActivationType.SAME_AS_PARENT)
		{
			if (m_aWaypointGroupNames.IsEmpty() && !m_sWPToSpawn)
				return;

			ScriptInvoker invoker;
			if (area)
				invoker = area.GetOnAreaInit();

			if (invoker)
				invoker.Insert(SetWPGroup);
		}

		if (m_eActivationType != activation)
			return;
		if (area)
			myarea = area;
		SCR_AIGroup.IgnoreSpawning(true);
		super.Init(myarea, activation);
		if (activation != SCR_ScenarioFrameworkEActivationType.SAME_AS_PARENT)
			SetWPGroup();
	}
	//------------------------------------------------------------------------------------------------
	void SetAIPrefabsForRemoval(array<ResourceName> arrayForRemoval)
	{
		m_aAIPrefabsForRemoval = arrayForRemoval;
	}

	//------------------------------------------------------------------------------------------------
	array<ResourceName> GetAIPrefabsForRemoval()
	{
		return m_aAIPrefabsForRemoval;
	}

	//------------------------------------------------------------------------------------------------
	void DecreaseAIGroupMemberCount(SCR_AIGroup group, AIAgent agent)
	{
		if (group.GetAgentsCount() == 0)
			SetIsTerminated(true);

		IEntity agentEntity = agent.GetControlledEntity();
		if (!agentEntity)
		{
			Print("ScenarioFramework: Decreasing AI member count failed due to not getting AI entity!", LogLevel.ERROR);
			return;
		}

		EntityPrefabData prefabData = agentEntity.GetPrefabData();
		if (!prefabData)
		{
			Print("ScenarioFramework: Decreasing AI member count failed due to not getting entity prefab data", LogLevel.ERROR);
			return;
		}

		ResourceName resource = prefabData.GetPrefabName();
		if (resource)
			m_aAIPrefabsForRemoval.Insert(resource);
	}

	//------------------------------------------------------------------------------------------------
	protected void CreateAIGroup()
	{
		EntitySpawnParams paramsPatrol = new EntitySpawnParams();
		paramsPatrol.TransformMode = ETransformMode.WORLD;
		paramsPatrol.Transform[3] = m_Entity.GetOrigin();
		Resource groupResource = Resource.Load(m_sGroupPrefab);
		if (!groupResource.IsValid())
			return;
		
		m_AIGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, null, paramsPatrol));
		if (!m_AIGroup)
			return;
		
		FactionAffiliationComponent facComp = FactionAffiliationComponent.Cast(m_Entity.FindComponent(FactionAffiliationComponent));
		if (!facComp)
			return;

		m_AIGroup.SetFaction(facComp.GetAffiliatedFaction());
		m_AIGroup.AddAIEntityToGroup(m_Entity, 0);
		m_AIGroup.inited = true;
	}

	//------------------------------------------------------------------------------------------------
	protected AIWaypoint CreateDefaultWaypoint()
	{
		if (!m_Entity)
			return null;

		EntitySpawnParams paramsPatrolWP = new EntitySpawnParams();
		paramsPatrolWP.TransformMode = ETransformMode.WORLD;
		paramsPatrolWP.Transform[3] = m_Entity.GetOrigin();

		Resource resWP = Resource.Load(m_sWPToSpawn);
		if (!resWP)
			return null;

		AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resWP, null, paramsPatrolWP));
		if (!waypoint)
			return null;

		m_aWaypoints.Insert(waypoint);
		return waypoint;
	}

	//------------------------------------------------------------------------------------------------
	protected void GetWaypointsFromLayer(notnull SCR_ScenarioFrameworkLayerBase layer, bool bRandomOrder)
	{
		//array<SCR_ScenarioFrameworkLayerBase> aChildEnts = layer.GetChildrenEntities();
		array<IEntity> aEnts = layer.GetSpawnedEntities();
		if (aEnts.IsEmpty())
		{
			Print("ScenarioFramework: no waypoints found!", LogLevel.WARNING);
			return;
		}
		m_aWaypoints.Resize(aEnts.Count());
		int iIndex = -1;

		foreach (IEntity entity : aEnts)
		{
			if (!AIWaypoint.Cast(entity))
				continue;
			
			if (bRandomOrder)
			{
				Math.Randomize(- 1);
				iIndex = Math.RandomInt(0, aEnts.Count());
				while (m_aWaypoints[iIndex])
				{
					iIndex = Math.RandomInt(0, aEnts.Count());
				}
			}
			else
			{
				iIndex++;
			}
			m_aWaypoints.Set(iIndex, AIWaypoint.Cast(entity));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void AddCycleWaypoint()
	{
		EntitySpawnParams paramsPatrolWP = new EntitySpawnParams();
		paramsPatrolWP.TransformMode = ETransformMode.WORLD;
		paramsPatrolWP.Transform[3] = m_aWaypoints[0].GetOrigin();
		Resource resWP = Resource.Load(m_sCycleWPPrefab);
		if (resWP)
			m_aWaypoints.Insert(AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resWP, null, paramsPatrolWP)));
	}
	void InitEnt()
	{
		if (!m_Entity)
			return;
		
		ScriptedDamageManagerComponent objectDmgManager = ScriptedDamageManagerComponent.Cast(m_Entity.FindComponent(ScriptedDamageManagerComponent));
		if (objectDmgManager)
			objectDmgManager.GetOnDamageStateChanged().Insert(OnObjectDamage);
	}
	//------------------------------------------------------------------------------------------------
	void SetWPGroup()
	{
		if (!m_Entity)
		{
			Print("ScenarioFramework: Trying to add waypoints to non existing entity! Did you select the object to spawn?", LogLevel.WARNING);
			SCR_AIGroup.IgnoreSpawning(false);
			return;
		}
		SP_AIDirector director = SP_AIDirector.Cast(m_Entity);
		if (director)
			return;
		if (!m_aWaypointGroupNames.IsEmpty())
		{
			//Select random layer which holds the waypoints (defined in the layer setting)

			SCR_WaypointSet wrapper = m_aWaypointGroupNames.GetRandomElement();
			IEntity entity = GetGame().GetWorld().FindEntityByName(wrapper.m_sName);
			if (entity)
			{

				SCR_ScenarioFrameworkSlotBase waypoint = SCR_ScenarioFrameworkSlotBase.Cast(entity.FindComponent(SCR_ScenarioFrameworkSlotBase));
				if (waypoint)
				{
					if (AIWaypoint.Cast(waypoint.GetSpawnedEntity()))
						m_aWaypoints.Insert(AIWaypoint.Cast(waypoint.GetSpawnedEntity()));
				}
				else
				{
					SCR_ScenarioFrameworkLayerBase WPGroupLayer = SCR_ScenarioFrameworkLayerBase.Cast(entity.FindComponent(SCR_ScenarioFrameworkLayerBase));
					if (WPGroupLayer)
						GetWaypointsFromLayer(WPGroupLayer, wrapper.m_bUseRandomOrder);
				}
				if (wrapper.m_bCycleWaypoints && !m_aWaypoints.IsEmpty())
					AddCycleWaypoint();
			}
		}
		else
		{
			CreateDefaultWaypoint();
		}

		m_AIGroup = SCR_AIGroup.Cast(m_Entity);
		SCR_AIGroup.IgnoreSpawning(false);
		if (m_AIGroup)
			ActivateAI();
		else
		{
			CreateAIGroup();
			if (!m_AIGroup)
			return;
		}
		
		if (m_aWaypoints.IsEmpty())
			return;

		array<AIWaypoint> waypointsWithoutCycle = {};
		AIWaypointCycle cycleWaypoint;
		foreach (AIWaypoint waypointToAdd : m_aWaypoints)
		{
			if (!AIWaypointCycle.Cast(waypointToAdd))
				waypointsWithoutCycle.Insert(waypointToAdd);
			else
				cycleWaypoint = AIWaypointCycle.Cast(waypointToAdd);
		}
		
		if (waypointsWithoutCycle.IsEmpty()) 
		{
			Print("ScenarioFramework SlotAI: There are not enough waypoints!", LogLevel.WARNING);
			return;
		}
		
		if (cycleWaypoint)
		{
			cycleWaypoint.SetWaypoints(waypointsWithoutCycle);
			m_AIGroup.AddWaypoint(cycleWaypoint);
		}
		else
		{
			foreach (AIWaypoint waypoint : waypointsWithoutCycle)
			{
				m_AIGroup.AddWaypoint(waypoint);
			}
		}
		
		if (m_bSpawnAIOnWPPos && !m_aWaypoints.IsEmpty())
			m_Entity.SetOrigin(m_aWaypoints[m_aWaypoints.Count() - 1].GetOrigin());

	}

	//------------------------------------------------------------------------------------------------
	void ActivateAI()
	{
		if (!m_aAIPrefabsForRemoval.IsEmpty())
		{
			foreach (ResourceName prefabToRemove : m_aAIPrefabsForRemoval)
			{
				m_AIGroup.m_aUnitPrefabSlots.RemoveItem(prefabToRemove);
			}
			m_AIGroup.SetMaxUnitsToSpawn(m_AIGroup.GetMaxMembers() - m_aAIPrefabsForRemoval.Count());
		}
		else if (m_bBalanceOnPlayersCount)
		{
			int iMaxUnitsInGroup = m_AIGroup.m_aUnitPrefabSlots.Count();
			float iUnitsToSpawn = Math.Map(GetPlayersCount(), 1, GetMaxPlayersForGameMode(), Math.RandomInt(1, 3), iMaxUnitsInGroup);
			m_AIGroup.SetMaxUnitsToSpawn(iUnitsToSpawn);
		}

		m_AIGroup.SpawnUnits();
		m_AIGroup.GetOnAgentRemoved().Insert(DecreaseAIGroupMemberCount);
	}

#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	void SCR_ScenarioFrameworkSlotAI(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_iDebugShapeColor = ARGB(100, 0x00, 0x10, 0xFF);
	}
#endif
};
