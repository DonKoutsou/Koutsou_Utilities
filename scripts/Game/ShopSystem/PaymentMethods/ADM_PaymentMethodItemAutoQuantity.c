class ADM_PaymentMethodItemAutoQuantity: ADM_PaymentMethodBase
{
	[Attribute(defvalue: "", desc: "Prefab of item", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_ItemPrefab;
	
	int m_ItemQuantity;
	
	protected ResourceName m_CachedIcon;
	
	bool MissingQuantity()
	{
		if (!m_ItemQuantity)
			return true;
		return false;
	}
	void SetItemQ(ResourceName merch)
	{
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(merch);
		SP_RequestData Data = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
		if (Data)
			m_ItemQuantity = Data.GetWorth();
		SP_RequestAmmoData AmmoData = SP_RequestAmmoData.Cast(entry.GetEntityDataOfType(SP_RequestAmmoData));
		if (AmmoData)
			m_ItemQuantity = AmmoData.GetWorth();
	}
	
	ResourceName GetItemPrefab()
	{
		return m_ItemPrefab;
	}
	
	array<IEntity> GetPaymentItemsInInventory(SCR_InventoryStorageManagerComponent inventory)
	{
		if (!inventory) 
			return null;
		
		array<IEntity> allInventoryItems = {};
		inventory.GetItems(allInventoryItems);
		
		array<IEntity> desiredItems = {};
		foreach (IEntity inventoryItem : allInventoryItems)
		{
			EntityPrefabData prefabData = inventoryItem.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			
			if (prefabName == m_ItemPrefab) 
				desiredItems.Insert(inventoryItem);	
		}
		
		return desiredItems;
	}
	
	override bool CheckPayment(IEntity player, int quantity = 1)
	{
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		array<IEntity> paymentItems = this.GetPaymentItemsInInventory(inventory);
		if (!paymentItems) 
			return false;
		
		return (paymentItems.Count() >= m_ItemQuantity*quantity);
	}
	
	override bool CollectPayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer() || !CheckPayment(player)) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		// Keep track of what is removed, as it is removed.
		// cant use ReturnPayment() b/c then we would give EVERYTHING back,
		// if we fail in the middle of removing payments the player would get something for free.
		// versus only returning what was taken.
		array<ResourceName> removedItems = {};
		array<bool> didRemoveItems = {};
		array<IEntity> paymentItems = this.GetPaymentItemsInInventory(inventory);
		foreach (IEntity item : paymentItems)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			
			bool didRemoveItem = inventory.TryDeleteItem(item, null);
			didRemoveItems.Insert(didRemoveItem);
			
			if (didRemoveItem) 
				removedItems.Insert(prefabName);
			
			if (didRemoveItems.Count() == m_ItemQuantity*quantity) 
				break;
		}
		
		if (didRemoveItems.Contains(false) || didRemoveItems.Count() < m_ItemQuantity*quantity)
		{
			Print("Error! Couldn't remove items for payment. Returning items already taken", LogLevel.ERROR);
			
			foreach (ResourceName returnItemPrefab : removedItems)
			{
				IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(returnItemPrefab));
				bool inserted = ADM_Utils.InsertAutoEquipItem(inventory, item);
				if (!inserted) 
					Print("Error! Couldn't return an item for payment.", LogLevel.ERROR); //TODO: not sure best way to handle this if it occurs, maybe drop it to ground?
			}
			
			return false;
		}
			
		return true;
	}
	
	override bool ReturnPayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer()) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		bool returned = true;
		for (int i = 0; i < m_ItemQuantity * quantity; i++)
		{
			IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_ItemPrefab));
			bool insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
			if (!insertedItem) 
				returned = false;
		}
			
		return returned;
	}
	
	override string GetDisplayString(int quantity = 1)
	{
		if (!m_ItemQuantity)
		{
			SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
			SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(m_ItemPrefab);
			SP_RequestData Data = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
			if (Data)
				m_ItemQuantity = Data.GetWorth();
			SP_RequestAmmoData AmmoData = SP_RequestAmmoData.Cast(entry.GetEntityDataOfType(SP_RequestAmmoData));
			if (AmmoData)
				m_ItemQuantity = AmmoData.GetWorth();
		}
		return string.Format("%1 x%2", ADM_Utils.GetPrefabDisplayName(m_ItemPrefab), m_ItemQuantity * quantity);
	}
	
	override ResourceName GetDisplayEntity()
	{
		return m_ItemPrefab;
	}
}
[BaseContainerProps()]
class ADM_ShopMerchandiseAutoPrice: ADM_ShopMerchandise
{
	override array<ref ADM_PaymentMethodBase> GetRequiredPayment() 
	{ 
		foreach (ADM_PaymentMethodBase itemmethod : m_RequiredPayment)
		{
			ADM_PaymentMethodItemAutoQuantity AQ = ADM_PaymentMethodItemAutoQuantity.Cast(itemmethod);
			if (!AQ)
				continue;
			if (AQ.MissingQuantity())
			{
				AQ.SetItemQ(m_Merchandise.GetPrefab());
			}
		}
		return m_RequiredPayment;
	}
}