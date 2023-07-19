class SP_PlaceEntityToSlot : ScriptedUserAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_CompatiblePrefab;
	
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_CompatiblePrefab);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		inv.FindItems(entitiesToDrop, pred);
		IEntity CookingPot = entitiesToDrop[0];
		array<Managed> outComponents = {};
		pOwnerEntity.FindComponents(BaseSlotComponent, outComponents);
		BaseSlotComponent potslot;
		foreach (Managed slot : outComponents)
		{
			BaseSlotComponent newslot = BaseSlotComponent.Cast(slot);
			potslot = newslot;
		}
		InventoryItemComponent pInvComp = InventoryItemComponent.Cast(CookingPot.FindComponent(InventoryItemComponent));
		InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
		bool removed = inv.TryRemoveItemFromStorage(CookingPot,parentSlot.GetStorage());
		if (removed)
			{
				potslot.GetSlotInfo().AttachEntity(CookingPot);
			}
		SCR_FireplaceComponent fire = SCR_FireplaceComponent.Cast(pOwnerEntity.FindComponent(SCR_FireplaceComponent));
		fire.OnPotAtatched();
	}
	
	event override bool CanBeShownScript(IEntity user) 
	{ 
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_CompatiblePrefab);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		inv.FindItems(entitiesToDrop, pred);
		if(entitiesToDrop.Count() > 0)
		{
			return true;
		}
		m_sCannotPerformReason = "No cooking pot in inventory";
		return false;
	}
}