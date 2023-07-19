class KLM_MineAction : KLM_BaseTraderAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_PrefabToSell;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_SellingAmount;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;	
	
	override bool Sell(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		vector mat[4];
		pOwnerEntity.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		
		Resource res = Resource.Load(m_PrefabToSell);
		if (res)
		{
			for (int j = 0; j < m_SellingAmount; j++)		
				GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);
			return true;
		}
		return false;
	}
	override bool CanBePerformedScript(IEntity user)
	{
		//SCR_InventoryStorageManagerComponent manager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		//BaseInventoryStorageComponent storagecomp = BaseInventoryStorageComponent.Cast(manager.FindStorageForItem(m_PrefabToSell));
		//if(manager.TrySpawnPrefabToStorage(m_PrefabToSell, storagecomp))
		//{
		//	return false;
		//}
		return true;
	}
}
