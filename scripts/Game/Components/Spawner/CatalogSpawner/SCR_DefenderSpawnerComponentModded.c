modded class SCR_DefenderSpawnerComponent
{

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