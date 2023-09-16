class DeadBodySmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(Owner);
		ScriptedDamageManagerComponent DMGMan = ScriptedDamageManagerComponent.Cast(char.GetDamageManager());
		if (!DMGMan.IsDestroyed())
		{
			return false;
		}
		
		/*WalletEntity wallet = char.GetWallet();
		if (wallet)
		{
			int ammount = wallet.GetCurrencyAmmount();
			if (ammount > 0)
			{
				return true;
			}
		}*/
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(Owner.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		foreach (IEntity item : items)
		{
			SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
			if (Unretr)
			{
				return true;
			}
			MagazineComponent mag = MagazineComponent.Cast(item.FindComponent(MagazineComponent));
			if (mag)
			{
				SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(Owner.FindComponent(SCR_AICombatComponent));
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
								return true;
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
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(Owner.FindComponent(EquipedLoadoutStorageComponent)); 
				LoadoutAreaType type = cloth.GetAreaType();
				if (!type || type.Type() == LoadoutWatchArea || type.Type() == LoadoutBinocularsArea)
				{
					continue;
				}
				if (!loadoutStorage.GetClothFromArea(type.Type()))
				{
					return true;
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
					EquipedLoadoutStorageComponent myloadoutStorage = EquipedLoadoutStorageComponent.Cast(User.FindComponent(EquipedLoadoutStorageComponent)); 
					IEntity	Helmet = myloadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
					if (Helmet)
					{
						EntityPrefabData prefabData = Helmet.GetPrefabData();
						ResourceName prefabName = prefabData.GetPrefabName();
						SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
						if(!entry)
						{
							return true;
						}
					}
				}
			}
		}
		SCR_CharacterInventoryStorageComponent MyloadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(User.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(Owner.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_InventoryStorageManagerComponent Myinv = SCR_InventoryStorageManagerComponent.Cast(User.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(Owner.FindComponent(SCR_InventoryStorageManagerComponent));
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
				needhelm = true;
			}
			if (needhelm)
			{
				IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
				if (Helmet)
				{
					return true;
				}
			}
			IEntity MyBackpack = MyloadoutStorage.GetClothFromArea(LoadoutBackpackArea);
			if (!MyBackpack)
			{
				IEntity Backpack = loadoutStorage.GetClothFromArea(LoadoutBackpackArea);
				if (Backpack)
				{
					return true;
				}
			}
			IEntity MyArmVest = MyloadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
			if (!MyArmVest)
			{
				IEntity ArmVest = loadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
				if (ArmVest)
				{
					return true;
				}
			}
			IEntity MyVest = MyloadoutStorage.GetClothFromArea(LoadoutVestArea);
			if (!MyVest)
			{
				IEntity Vest = loadoutStorage.GetClothFromArea(LoadoutVestArea);
				if (Vest)
				{
					return true;
				}
			}
			IEntity MyJacket = MyloadoutStorage.GetClothFromArea(LoadoutJacketArea);
			if (!MyJacket)
			{
				IEntity Jacket = loadoutStorage.GetClothFromArea(LoadoutJacketArea);
				if (Jacket)
				{
					return true;
				}
			}
			IEntity MyPants = MyloadoutStorage.GetClothFromArea(LoadoutPantsArea);
			if (!MyPants)
			{
				IEntity Pants = loadoutStorage.GetClothFromArea(LoadoutPantsArea);
				if (Pants)
				{
					return true;
				}
			}
			IEntity MyBoots = MyloadoutStorage.GetClothFromArea(LoadoutBootsArea);
			if (!MyBoots)
			{
				IEntity Boots = loadoutStorage.GetClothFromArea(LoadoutBootsArea);
				if (Boots)
				{
					return true;
				}
			}
		}
		
		return false;
	}
}