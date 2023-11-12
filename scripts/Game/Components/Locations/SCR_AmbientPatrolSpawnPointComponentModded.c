
//------------------------------------------------------------------------------------------------
modded class SCR_AmbientPatrolSpawnPointComponent
{
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(SCR_Faction faction)
	{
		if (faction.GetFactionKey() == "RENEGADE")
		{
			UpdateRene(faction);
			return;
		}
		super.Update(faction);
		
	}
	protected void UpdateRene(SCR_Faction faction)
	{
		if (!m_Waypoint)
			PrepareWaypoints();
		
		m_SavedFaction = faction;
		
		if (!faction)
			return;
		
		SCR_EntityCatalog entityCatalog = faction.GetFactionEntityCatalogOfType(EEntityCatalogType.GROUP);
		
		if (!entityCatalog)
			return;
		
		array<SCR_EntityCatalogEntry> data = {};
		entityCatalog.GetEntityListWithData(SCR_EntityCatalogAmbientPatrolData, data);
		
		if (m_bPickRandomGroupType)
		{
			m_sPrefab = GetRandomPrefabByProbability(entityCatalog, data);
			return;
		}
		
		SCR_EntityCatalogEntry catalogEntry;
		SCR_EntityCatalogAmbientPatrolData patrolData;
		array <ResourceName> possibleprefabs = {};
		for (int i = 0, count = data.Count(); i < count; i++)
		{
			int catalogIndex = data[i].GetCatalogIndex();
			catalogEntry = entityCatalog.GetCatalogEntry(catalogIndex);
			
			if (!catalogEntry)
				continue;
			
			patrolData = SCR_EntityCatalogAmbientPatrolData.Cast(catalogEntry.GetEntityDataOfType(SCR_EntityCatalogAmbientPatrolData));
			
			if (!patrolData)
				continue;
			
			if (patrolData.GetGroupType() != SCR_EGroupType.SOLOGUY)
				continue;
			
			possibleprefabs.Insert(catalogEntry.GetPrefab());
		}
		if (!possibleprefabs.IsEmpty())
		{
			m_sPrefab = possibleprefabs.GetRandomElement();
		}
	}
};