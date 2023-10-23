modded class SCR_ResupplySupportStationComponent
{
	int GetSupplyCostWithPrefab(IEntity actionOwner, IEntity actionUser, ResourceName Prefab)
	{
		if (!IsUsingSupplies())
			return 0;
		
		
		SCR_EntityCatalogEntry catalogEntry = m_EntityCatalogManager.GetEntryWithPrefabFromAnyCatalog(EEntityCatalogType.ITEM, Prefab, GetFaction());
		if (!catalogEntry)
		{
			Print("'SCR_ResupplySupportStationComponent' could not find SCR_EntityCatalogEntry for '" + Prefab + "' so will use fallback cost!", LogLevel.WARNING);
			return Math.ClampInt(m_iFallbackItemSupplyCost + m_iBaseSupplyCostOnUse, 0, int.MAX);
		}
		
		//~ Could not find arsenal data use fallback cost
		SCR_ArsenalItem arsenalData = SCR_ArsenalItem.Cast(catalogEntry.GetEntityDataOfType(SCR_ArsenalItem));
		if (!arsenalData)
		{
			Print("'SCR_ResupplySupportStationComponent' could not find SCR_ArsenalItem '" + Prefab + "' so will use fallback cost!", LogLevel.WARNING);
			return Math.ClampInt(m_iFallbackItemSupplyCost + m_iBaseSupplyCostOnUse, 0, int.MAX);
		}
		
		return Math.ClampInt(arsenalData.GetSupplyCost() + m_iBaseSupplyCostOnUse, 0, int.MAX);
	}
};
