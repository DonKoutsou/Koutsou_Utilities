modded class SCR_DefenderSpawnerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Spawn defender AI group from faction prefab. Also handles price per spawned unit.
	//! Should some units be despawned because of player distance, spawn price will be reduced by it
	override protected void SpawnGroup()
	{
		if (IsProxy())
			return;

		if (!m_GroupEntry)
			return;

		SCR_EntityCatalogSpawnerData entityData = SCR_EntityCatalogSpawnerData.Cast(m_GroupEntry.GetEntityDataOfType(SCR_EntityCatalogSpawnerData));
		if (!entityData)
			return;

		float supplies = GetSpawnerSupplies();
		int spawnCost = entityData.GetSupplyCost();
		if (supplies < entityData.GetSupplyCost() && m_bSuppliesConsumptionEnabled)
			return;

		SCR_EntitySpawnerSlotComponent slot = GetFreeSlot();
		if (!slot)
			return;

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		slot.GetOwner().GetTransform(params.Transform);

		Resource res = Resource.Load(m_GroupEntry.GetPrefab());
		if (!res.IsValid())
			return;

		SCR_AIGroup.IgnoreSpawning(true);
		SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(res, null, params));
		if (!group || group.m_aUnitPrefabSlots.Count() == 0)
		{
			SCR_AIGroup.IgnoreSpawning(false);
			return;
		}

		m_AIgroup = group;
		
		if (m_OnDefenderGroupSpawned)
			m_OnDefenderGroupSpawned.Invoke(this, group);

		//waypoint handling
		if (m_Waypoint)
		{
			group.AddWaypoint(m_Waypoint);
			return;
		}

		vector spawnposMat[4];
		Resource wpRes;
		SCR_AIWaypoint wp;

		SCR_EntityLabelPointComponent rallyPoint = slot.GetRallyPoint();
		if (rallyPoint)
			rallyPoint.GetOwner().GetTransform(params.Transform);

		SCR_DefenderSpawnerComponentClass prefabData = SCR_DefenderSpawnerComponentClass.Cast(GetComponentData(GetOwner()));
		if (!prefabData)
			return;

		wpRes = Resource.Load(prefabData.m_sDefaultWaypointPrefab);
		if (!wpRes.IsValid())
			return;

		wp = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefabLocal(wpRes, null, params));
		if (!wp)
			return;
		
		wp.SetCompletionRadius(150);

		group.AddWaypoint(wp);
		
		
		
		ReinforceGroup();
	}
	SCR_AIGroup SpawnGroupExtrarnal()
	{
		if (IsProxy())
			return null;

		if (!m_GroupEntry)
			return null;

		SCR_EntityCatalogSpawnerData entityData = SCR_EntityCatalogSpawnerData.Cast(m_GroupEntry.GetEntityDataOfType(SCR_EntityCatalogSpawnerData));
		if (!entityData)
			return null;


		SCR_EntitySpawnerSlotComponent slot = GetFreeSlot();
		if (!slot)
			return null;

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		slot.GetOwner().GetTransform(params.Transform);

		Resource res = Resource.Load(m_GroupEntry.GetPrefab());
		if (!res.IsValid())
			return null;

		SCR_AIGroup.IgnoreSpawning(true);
		SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(res, null, params));
		if (!group || group.m_aUnitPrefabSlots.Count() == 0)
		{
			SCR_AIGroup.IgnoreSpawning(false);
			return null;
		}

		m_AIgroup = group;

		SCR_EntityLabelPointComponent rallyPoint = slot.GetRallyPoint();
		if (rallyPoint)
			rallyPoint.GetOwner().GetTransform(params.Transform);
		return m_AIgroup;
	};
};