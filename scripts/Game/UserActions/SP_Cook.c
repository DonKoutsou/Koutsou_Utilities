class SP_Cook : ScriptedUserAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_RawMeatPrefab;
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_CookedMeatPrefab;
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_BurnedMeatPrefab;

	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_FireplaceComponent fireplace = SCR_FireplaceComponent.Cast(pOwnerEntity.GetParent() 	.FindComponent(SCR_FireplaceComponent));
		if(!fireplace)
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom("Cooking pot needs to placed on a fireplace to be able to cook");
			return;
		}
		PrefabResource_Predicate Rawpred = new PrefabResource_Predicate(m_RawMeatPrefab);
		PrefabResource_Predicate Cookedpred = new PrefabResource_Predicate(m_CookedMeatPrefab);
		array<IEntity> RawMeat = new array<IEntity>;
		array<IEntity> CookedMeat = new array<IEntity>;
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		BaseInventoryStorageComponent pInvComp = BaseInventoryStorageComponent.Cast(pOwnerEntity.FindComponent(BaseInventoryStorageComponent));
		inv.FindItems(RawMeat, Rawpred);
		inv.FindItems(CookedMeat, Cookedpred);
		if(!CookedMeat && !RawMeat)
		{
			return;
		}
		foreach(IEntity MeatPiece : CookedMeat)
		{
			bool removed = inv.TryRemoveItemFromStorage(MeatPiece,pInvComp);
			if (removed == true)
			{
				RplComponent.Cast(MeatPiece.FindComponent(RplComponent)).DeleteRplEntity(MeatPiece, true);
				inv.TrySpawnPrefabToStorage(m_BurnedMeatPrefab, pInvComp);
			}
		}
		
		foreach(IEntity MeatPiece : RawMeat)
		{
			bool removed = inv.TryRemoveItemFromStorage(MeatPiece,pInvComp);
			if (removed == true)
			{
				RplComponent.Cast(MeatPiece.FindComponent(RplComponent)).DeleteRplEntity(MeatPiece, true);
				inv.TrySpawnPrefabToStorage(m_CookedMeatPrefab, pInvComp);
			}
		}
		
	}
}