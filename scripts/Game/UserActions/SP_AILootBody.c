class SP_AILootBodyAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		InventoryStorageManagerComponent stocompKiller = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		foreach (IEntity item : items)
		{
			SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
			SCR_ConsumableItemComponent cons = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
			MagazineComponent Mag = MagazineComponent.Cast(item.FindComponent(MagazineComponent));
			if (Unretr || cons || Mag)
			{
				inv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
			}
		}
		SCR_CharacterInventoryStorageComponent MyloadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(pUserEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_InventoryStorageManagerComponent Myinv = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (loadoutStorage && MyloadoutStorage)
		{
			IEntity MyHelmet = MyloadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
			if (!MyHelmet)
			{
				IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
				if (Helmet)
				{
					Ownerinv.TryRemoveItemFromInventory(Helmet);
					Myinv.EquipCloth(Helmet);
				}
			}
			IEntity MyBackpack = MyloadoutStorage.GetClothFromArea(LoadoutBackpackArea);
			if (!MyBackpack)
			{
				IEntity Backpack = loadoutStorage.GetClothFromArea(LoadoutBackpackArea);
				if (Backpack)
				{
					Ownerinv.TryRemoveItemFromInventory(Backpack);
					Myinv.EquipCloth(Backpack);
				}
			}
			IEntity MyArmVest = MyloadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
			if (!MyArmVest)
			{
				IEntity ArmVest = loadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
				if (ArmVest)
				{
					Ownerinv.TryRemoveItemFromInventory(ArmVest);
					Myinv.EquipCloth(ArmVest);
				}
			}
			IEntity MyVest = MyloadoutStorage.GetClothFromArea(LoadoutVestArea);
			if (!MyVest)
			{
				IEntity Vest = loadoutStorage.GetClothFromArea(LoadoutVestArea);
				if (Vest)
				{
					Ownerinv.TryRemoveItemFromInventory(Vest);
					Myinv.EquipCloth(Vest);
				}
			}
			IEntity MyJacket = MyloadoutStorage.GetClothFromArea(LoadoutJacketArea);
			if (!MyJacket)
			{
				IEntity Jacket = loadoutStorage.GetClothFromArea(LoadoutJacketArea);
				if (Jacket)
				{
					Ownerinv.TryRemoveItemFromInventory(Jacket);
					Myinv.EquipCloth(Jacket);
				}
			}
			IEntity MyPants = MyloadoutStorage.GetClothFromArea(LoadoutPantsArea);
			if (!MyPants)
			{
				IEntity Pants = loadoutStorage.GetClothFromArea(LoadoutPantsArea);
				if (Pants)
				{
					Ownerinv.TryRemoveItemFromInventory(Pants);
					Myinv.EquipCloth(Pants);
				}
			}
			IEntity MyBoots = MyloadoutStorage.GetClothFromArea(LoadoutBootsArea);
			if (!MyBoots)
			{
				IEntity Boots = loadoutStorage.GetClothFromArea(LoadoutBootsArea);
				if (Boots)
				{
					Ownerinv.TryRemoveItemFromInventory(Boots);
					Myinv.EquipCloth(Boots);
				}
			}
		}
		SP_CharacterAISmartActionComponent SmartComp = SP_CharacterAISmartActionComponent.Cast(pOwnerEntity.FindComponent(SP_CharacterAISmartActionComponent));
		if(SmartComp)
			{
				SmartComp.SetActionAccessible(false);
			}
	}
	
}