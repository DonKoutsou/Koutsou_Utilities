class SP_AILootBodyAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		/*WalletEntity wallet = char.GetWallet();
		if (wallet)
		{
			int ammount = wallet.GetCurrencyAmmount();
			if (ammount > 0)
			{
				SCR_ChimeraCharacter Uchar = SCR_ChimeraCharacter.Cast(pUserEntity);
				WalletEntity Uwallet = char.GetWallet();
				Uwallet.TakeCurrency(wallet, ammount);
			}
		}*/
		SCR_InventoryStorageManagerComponent Myinv = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		
		array<IEntity> items = new array<IEntity>();
		Ownerinv.GetItems(items);
		InventoryStorageManagerComponent stocompKiller = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		foreach (IEntity item : items)
		{
			SCR_ConsumableItemComponent cons = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
			if (cons)
			{
				Myinv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
				continue;
			}
			SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
			if (Unretr)
			{
				Myinv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
				continue;
			}
			MagazineComponent mag = MagazineComponent.Cast(item.FindComponent(MagazineComponent));
			if (mag)
			{
				SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(pUserEntity.FindComponent(SCR_AICombatComponent));
				BaseWeaponComponent weaponComp;
				BaseMagazineComponent magazineComp;
				int muzzleId;
				weaponComp = CombatComp.GetCurrentWeapon();
	
				if (weaponComp)
				{
					if (weaponComp.GetCurrentMagazine())
					{
						magazineComp = weaponComp.GetCurrentMagazine();
						if (magazineComp)
						{
							if (magazineComp.GetMagazineWell() == mag.GetMagazineWell())
							{
								Myinv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
								continue;
							}
						}
					}
				}
			}
			//clothes
			BaseLoadoutClothComponent cloth = BaseLoadoutClothComponent.Cast(item.FindComponent(BaseLoadoutClothComponent));
			if (cloth)
			{
				//if i miss this part of clothing it means i have something to loot from here
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(pOwnerEntity.FindComponent(EquipedLoadoutStorageComponent)); 
				LoadoutAreaType type = cloth.GetAreaType();
				if (!type || type.Type() == LoadoutWatchArea)
				{
					continue;
				}
				if (!loadoutStorage.GetClothFromArea(type.Type()))
				{
					Ownerinv.TryRemoveItemFromInventory(item);
					Myinv.EquipCloth(item);
					continue;
				}
				//look for armored helmets
				if (cloth.GetAreaType().Type() == LoadoutHeadCoverArea)
				{
					SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
					SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
					//figure out if its helmet
					EntityPrefabData ClothprefabData = item.GetPrefabData();
					ResourceName ClothprefabName = ClothprefabData.GetPrefabName();
					SCR_EntityCatalogEntry Clothentry = RequestCatalog.GetEntryWithPrefab(ClothprefabName);
					if(!Clothentry)
					{
						continue;
					}
					//see if i need helmet
					EquipedLoadoutStorageComponent myloadoutStorage = EquipedLoadoutStorageComponent.Cast(pUserEntity.FindComponent(EquipedLoadoutStorageComponent)); 
					IEntity	Helmet = myloadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
					if (Helmet)
					{
						EntityPrefabData prefabData = Helmet.GetPrefabData();
						ResourceName prefabName = prefabData.GetPrefabName();
						SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
						if(!entry)
						{
							Ownerinv.TryRemoveItemFromInventory(item);
							Myinv.EquipCloth(item);
							continue;
						}
					}
				}
			}
		}
		SCR_CharacterInventoryStorageComponent MyloadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(pUserEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
		
		if (loadoutStorage && MyloadoutStorage)
		{
			IEntity MyHelmet = MyloadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
			bool needhelm;
			if (MyHelmet)
			{
				EntityPrefabData prefabData = MyHelmet.GetPrefabData();
				ResourceName prefabName = prefabData.GetPrefabName();
				SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
				SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
				SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
				if(entry)
				{
					needhelm = false;
				}
				else
					needhelm = true;
			}
			if (needhelm)
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
	override event bool CanBeShownScript(IEntity user) { return false; };
	
}
class SP_AIAmmoRefilAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_InventoryStorageManagerComponent InvMan = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!InvMan)
			return;
		SCR_InventoryStorageManagerComponent UserInvMan = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!UserInvMan)
			return;
		SCR_AICombatComponent m_CombatComp = SCR_AICombatComponent.Cast(pUserEntity.FindComponent(SCR_AICombatComponent));
		if (!m_CombatComp)
			return;
		BaseWeaponComponent weaponComp;
		BaseMagazineComponent magazineComp;
		int muzzleId;
		
		weaponComp = m_CombatComp.GetCurrentWeapon();
	
		if (weaponComp)
		{
			if (weaponComp.GetCurrentMagazine())
			{
				magazineComp = weaponComp.GetCurrentMagazine();
			}
		}
		BaseMagazineWell iMagWell = magazineComp.GetMagazineWell();
		SCR_MagazinePredicate pred = new SCR_MagazinePredicate();
		pred.magWellType = iMagWell.Type();
		array <IEntity> OutItems = {};
		InvMan.FindItems(OutItems, pred);
		if (OutItems.IsEmpty())
			return;
		foreach (IEntity item : OutItems)
		{
			UserInvMan.TryMoveItemToStorage(item, UserInvMan.FindStorageForItem(item));
		}
		SP_CharacterAISmartActionComponent SmartComp = SP_CharacterAISmartActionComponent.Cast(pOwnerEntity.FindComponent(SP_CharacterAISmartActionComponent));
		if(SmartComp)
			{
				SmartComp.SetActionAccessible(false);
			}
	}
	override event bool CanBeShownScript(IEntity user) { return false; };
	
}
modded class SCR_AICombatComponent
{
	BaseWeaponComponent GetCurrentWeapon()
	{
		BaseWeaponComponent currentWeapon = null;
		if (m_CurrentTurretController)
		{
			// If we are in turret, find turret weapon
			BaseWeaponManagerComponent turretWpnMgr = m_CurrentTurretController.GetWeaponManager();
			if (turretWpnMgr)
				currentWeapon = turretWpnMgr.GetCurrentWeapon();
		}
		else if (m_WpnManager)
		{
			// If not in turret, use character's weapon
			currentWeapon = m_WpnManager.GetCurrentWeapon();
		}
		
		if (currentWeapon)
			return currentWeapon;
		
		return null;
	}
}