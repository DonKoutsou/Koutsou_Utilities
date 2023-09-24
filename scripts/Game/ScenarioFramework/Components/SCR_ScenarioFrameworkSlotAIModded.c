modded class SCR_ScenarioFrameworkSlotAI
{
	EEntityCatalogType GetEntityCatalogueType()
	{
		return m_eEntityCatalogType;
	}
	void SetEntityCatalogueType(EEntityCatalogType Type)
	{
		m_eEntityCatalogType = Type;
	}
	void ClearEntityLabels()
	{
		m_aIncludedEditableEntityLabels.Clear();
		m_aExcludedEditableEntityLabels.Clear();
	}
	void ClearPrefabtoSpawn()
	{
		m_sObjectToSpawn = STRING_EMPTY;
	}
	void ClearRandomPrefab()
	{
		m_sRandomlySpawnedObject = STRING_EMPTY;
	}
	void SetEntityLabels(array<EEditableEntityLabel> Included, array<EEditableEntityLabel> Excluded)
	{
		m_aIncludedEditableEntityLabels.Copy(Included);
		m_aExcludedEditableEntityLabels.Copy(Excluded);
	}
	override protected void CreateAIGroup()
	{
		EntitySpawnParams paramsPatrol = new EntitySpawnParams();
		paramsPatrol.TransformMode = ETransformMode.WORLD;
		
		paramsPatrol.Transform[3] = m_Entity.GetOrigin();
		Resource groupResource = Resource.Load(m_sGroupPrefab);
		if (!groupResource.IsValid())
			return;
		
		m_AIGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), paramsPatrol));
		if (!m_AIGroup)
			return;
		
		FactionAffiliationComponent facComp = FactionAffiliationComponent.Cast(m_Entity.FindComponent(FactionAffiliationComponent));
		if (!facComp)
			return;

		m_AIGroup.SetFaction(facComp.GetAffiliatedFaction());
		m_AIGroup.moddedAddAIEntityToGroup(m_Entity, true);
		
		if (m_vPosition != vector.Zero)
			m_Entity.SetOrigin(m_vPosition);
	}
	void InitEnt()
	{
		if (!m_Entity)
			return;
		
		ScriptedDamageManagerComponent objectDmgManager = ScriptedDamageManagerComponent.Cast(m_Entity.FindComponent(ScriptedDamageManagerComponent));
		if (objectDmgManager)
			objectDmgManager.GetOnDamageStateChanged().Insert(OnObjectDamage);
	}
	override void SetWPGroup()
	{	
		if (!m_Entity)
		{
			Print(string.Format("ScenarioFramework: Trying to add waypoints to non existing entity! Did you select the object to spawn for %1?", GetOwner().GetName()), LogLevel.ERROR);
			SCR_AIGroup.IgnoreSpawning(false);
			return;
		}
		SP_AIDirector director = SP_AIDirector.Cast(m_Entity);
		if (director)
			return;
			   
		  
		if (!m_aWaypointGroupNames.IsEmpty())
		{
			//Select random layer which holds the waypoints (defined in the layer setting)
		for (int i; i < m_aWaypointGroupNames.Count(); i++)
		{
			Math.Randomize(-1);
			SCR_WaypointSet wrapper = m_aWaypointGroupNames.Get(i);
			IEntity entity = GetGame().GetWorld().FindEntityByName(wrapper.m_sName);
			if (entity)
			{
				SCR_ScenarioFrameworkLayerBase waypointLayer = SCR_ScenarioFrameworkLayerBase.Cast(entity.FindComponent(SCR_ScenarioFrameworkLayerBase));
				if (waypointLayer)
				{
					SCR_ScenarioFrameworkSlotBase waypointSlot = SCR_ScenarioFrameworkSlotBase.Cast(waypointLayer);
					if (waypointSlot)
					{
						if (AIWaypoint.Cast(waypointSlot.GetSpawnedEntity()))
							m_aWaypoints.Insert(AIWaypoint.Cast(waypointSlot.GetSpawnedEntity()));
					}
					else
					{
						array<SCR_ScenarioFrameworkLayerBase> childSlots = {};
						childSlots = waypointLayer.GetChildrenEntities();
				
						foreach (SCR_ScenarioFrameworkLayerBase child : childSlots)
						{
							SCR_ScenarioFrameworkSlotBase waypoint = SCR_ScenarioFrameworkSlotBase.Cast(child);
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
						}
					
						if (wrapper.m_bCycleWaypoints && !m_aWaypoints.IsEmpty())
						AddCycleWaypoint();
					}
				}
															 
						
			}
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
			Print(string.Format("ScenarioFramework - SlotAI: There are not enough waypoints for %1!", GetOwner().GetName()), LogLevel.ERROR);
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
}