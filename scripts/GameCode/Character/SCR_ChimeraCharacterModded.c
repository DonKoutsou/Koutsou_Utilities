modded class SCR_ChimeraCharacter
{
	[Attribute("0")]
	bool IsImportantCharacter;
	
	WalletEntity GetWallet()
	{
		SCR_InventoryStorageManagerComponent InventoryManager = SCR_InventoryStorageManagerComponent.Cast(FindComponent(SCR_InventoryStorageManagerComponent));
		array <IEntity> wallets = {};
		SP_WalletPredicate pred = new SP_WalletPredicate();
		InventoryManager.FindItems(wallets, pred);
		for (int i, count = wallets.Count(); i < count; i++)
		{
			WalletEntity wallet = WalletEntity.Cast(wallets[i]);
				return wallet;
		}
		return null;
	}
	bool Checkneed(array <ERequestRewardItemDesctiptor> needs, out int ammount, out BaseMagazineComponent Mag = null)
	{
		int severity;
		foreach (ERequestRewardItemDesctiptor need : needs)
		{
			if (need == ERequestRewardItemDesctiptor.BANDAGE)
			{
				if (CheckIfMissingBandages(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.WEAPON)
			{
				if (CheckIfMissingWeapon(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.AMMO)
			{
				if (CheckIfMissingAmmo(ammount, severity, Mag))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.ARMOR)
			{
				if (CheckIfMissingArmor(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.HELMET)
			{
				if (CheckIfMissingHelmet(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.BACKPACK)
			{
				if (CheckIfMissingBackpack(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.LOAD_BEARING_SYSTEM)
			{
				if (CheckIfMissingLBS(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.SIDEARM)
			{
				if (CheckIfMissingSidearm(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.BINOCULARS)
			{
				if (CheckIfMissingBinoculars(ammount, severity))
				{
					return true;
				}
			}
			else if (need == ERequestRewardItemDesctiptor.EXPLOSIVE)
			{
				if (CheckIfMissingExplosives(ammount, severity))
				{
					return true;
				}
			}
			else if (CheckIfMissingItemOfType(need, ammount, severity))
			{
				return true;
			}
		}
		return false;
	}
	ERequestRewardItemDesctiptor GetNeed(out int ammount, out BaseMagazineComponent Mag = null)
	{
		map <ERequestRewardItemDesctiptor, int> descmap = new map <ERequestRewardItemDesctiptor, int>();
		int severity
		if (CheckIfMissingBandages(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.BANDAGE;
			//descmap.Insert(ERequestRewardItemDesctiptor.BANDAGE, ammount);
		}
		if (CheckIfMissingWeapon(ammount, severity))
		{
			return ERequestRewardItemDesctiptor.WEAPON;
			//descmap.Insert(ERequestRewardItemDesctiptor.WEAPON, ammount);
		}
		if (CheckIfMissingAmmo(ammount, severity, Mag))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.AMMO;
			//descmap.Insert(ERequestRewardItemDesctiptor.AMMO, ammount);
		}
		if (CheckIfMissingArmor(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.ARMOR;
			//descmap.Insert(ERequestRewardItemDesctiptor.ARMOR, ammount);
		}
		if (CheckIfMissingHelmet(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.HELMET;
			//descmap.Insert(ERequestRewardItemDesctiptor.HELMET, ammount);
		}
		if (CheckIfMissingBackpack(ammount, severity))
		{
		//	if (severity > 4)
			return ERequestRewardItemDesctiptor.BACKPACK;
			//descmap.Insert(ERequestRewardItemDesctiptor.BACKPACK, ammount);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.DRINK, ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.DRINK;
			//descmap.Insert( ERequestRewardItemDesctiptor.DRINK, ammount);
		}
		if (CheckIfMissingLBS(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.LOAD_BEARING_SYSTEM;
			//descmap.Insert(ERequestRewardItemDesctiptor.LOAD_BEARING_SYSTEM, ammount);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.FLASHLIGHT, ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.FLASHLIGHT;
			//descmap.Insert(ERequestRewardItemDesctiptor.FLASHLIGHT, ammount);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.COMPASS, ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.COMPASS;
			//descmap.Insert(ERequestRewardItemDesctiptor.COMPASS, ammount);
		}
		if (CheckIfMissingBinoculars(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.BINOCULARS;
			//descmap.Insert(ERequestRewardItemDesctiptor.BINOCULARS, ammount);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.MAP, ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.MAP;
			//descmap.Insert(ERequestRewardItemDesctiptor.MAP, ammount);
		}
		if (CheckIfMissingExplosives(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.EXPLOSIVE;
			//descmap.Insert(ERequestRewardItemDesctiptor.EXPLOSIVE, ammount);
		}
		if (CheckIfMissingSidearm(ammount, severity))
		{
			//if (severity > 4)
			return ERequestRewardItemDesctiptor.SIDEARM;
			//descmap.Insert(ERequestRewardItemDesctiptor.EXPLOSIVE, ammount);
		}
		return null;
	}
	
	bool CheckIfMissingBandages(out int ammount, out int severity)
	{
		bool missingbandages;
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_BandagePredicate pred = new SCR_BandagePredicate();
		array <IEntity> bandages = {};
		inventory.FindItems(bandages, pred);
		if (bandages.Count() < 2)
		{
			missingbandages = true;
			if (bandages.IsEmpty())
			{
				severity = 5;
				ammount = 2;
			}
				
			else
			{
				severity = 3;
				ammount = 1;
			}
		}
		else
		{
			severity = 0;
		}
		return missingbandages;
	}
	
	bool CheckIfMissingItemOfType(ERequestRewardItemDesctiptor Desc, out int ammount, out int severity)
	{
		bool missingItem;
		
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		
		array <SCR_EntityCatalogEntry> entries = {};
		
		RequestCatalog.GetRequestItems(Desc, null, entries);
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(FindComponent(SCR_InventoryStorageManagerComponent));
		
		array <IEntity> items = {};
		
		foreach (SCR_EntityCatalogEntry entry : entries)
		{
			SCR_PrefabNamePredicate pred = new SCR_PrefabNamePredicate();
			pred.prefabName = entry.GetPrefab();
			inventory.FindItems(items, pred);
		}
		if (items.Count() < 1)
		{
			missingItem = true;
			ammount = 1;
		}
		return missingItem;
	}
	
	bool CheckIfMissingHelmet(out int ammount, out int severity)
	{
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(FindComponent(EquipedLoadoutStorageComponent));
		if (!loadoutStorage)
			return false;

		IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
		if (!Helmet)
		{
			ammount = 1;
			return true;
		}
		if (Helmet)
		{
			ResourceName prefabName = Helmet.GetPrefabData().GetPrefabName();
			SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
			SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
			if(!entry)
			{
				ammount = 1;
				return true;
			}
		}
		return false;
	}
	
	bool CheckIfMissingBackpack(out int ammount, out int severity)
	{
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(FindComponent(EquipedLoadoutStorageComponent));
		if (!loadoutStorage)
			return false;
				
		IEntity Backpack = loadoutStorage.GetClothFromArea(LoadoutBackpackArea);
		if (!Backpack)
		{
			ammount = 1;
			return true;
		}
		return false;
	}
	
	bool CheckIfMissingBinoculars(out int ammount, out int severity)
	{
		return false;
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!loadoutStorage)
			return false;
		LoadoutBinocularsArea type = new LoadoutBinocularsArea();
		IEntity Binocs = loadoutStorage.GetItemFromLoadoutSlot(type).GetOwner();
		if (!Binocs)
		{
			ammount = 1;
			return true;
		}
		return false;
	}
	
	bool CheckIfMissingLBS(out int ammount, out int severity)
	{
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(FindComponent(EquipedLoadoutStorageComponent));
		if (!loadoutStorage)
			return false;
				
		IEntity LBS = loadoutStorage.GetClothFromArea(LoadoutVestArea);
		if (!LBS)
		{
			ammount = 1;
			return true;
		}
		return false;
	}
	
	bool CheckIfMissingArmor(out int ammount, out int severity)
	{
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(FindComponent(EquipedLoadoutStorageComponent));
		if (!loadoutStorage)
			return false;

		IEntity Vest = loadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
		if (!Vest)
		{
			ammount = 1;
			return true;
		}
		if (Vest)
		{
			EntityPrefabData prefabData = Vest.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
			SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
			if(!entry)
			{
				ammount = 1;
				return true;
			}
		}
		return false;
	}
	bool CheckIfMissingWeapon(out int ammount, out int severity)
	{
		SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(FindComponent(SCR_AICombatComponent));
		if (!CombatComp)
			return false;
		CharacterWeaponManagerComponent WeaponMan = CharacterWeaponManagerComponent.Cast(FindComponent(CharacterWeaponManagerComponent));
		if (!WeaponMan)
			return false;
		
		BaseWeaponComponent weaponComp;
		int muzzleId;
		
		array<IEntity> outWeapons = {};
		WeaponMan.GetWeaponsList(outWeapons);
		if (outWeapons.IsEmpty())
		{
			ammount = 1;
			return true;
		}
			
		return false;
	}
	bool CheckIfMissingExplosives (out int ammount, out int severity)
	{
		return false;
	}
	bool CheckIfMissingSidearm(out int ammount, out int severity)
	{
		SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(FindComponent(SCR_AICombatComponent));
		if (!CombatComp)
			return false;
		CharacterWeaponManagerComponent WeaponMan = CharacterWeaponManagerComponent.Cast(FindComponent(CharacterWeaponManagerComponent));
		if (!WeaponMan)
			return false;
		
		BaseWeaponComponent weaponComp;
		int muzzleId;
		array<WeaponSlotComponent> outSlots = {};
		WeaponMan.GetWeaponsSlots(outSlots);
		if (!outSlots[2].GetWeaponEntity())
		{
			ammount = 1;
			return true;
		}
			
		return false;
	}
	bool CheckIfMissingAmmo(out int ammount, out int severity, out BaseMagazineComponent magazinewell)
	{
		SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(FindComponent(SCR_AICombatComponent));
		if (!CombatComp)
			return false;
		CharacterWeaponManagerComponent WeaponMan = CharacterWeaponManagerComponent.Cast(FindComponent(CharacterWeaponManagerComponent));
		if (!WeaponMan)
			return false;
		
		BaseWeaponComponent weaponComp;
		int muzzleId;
		
		array<IEntity> outWeapons = {};
		WeaponMan.GetWeaponsList(outWeapons);
		if (outWeapons.IsEmpty())
			return false;
		bool lowAmmo;
		foreach (IEntity weapon : outWeapons)
		{
			if (lowAmmo)
				return lowAmmo;
			weaponComp = BaseWeaponComponent.Cast(weapon.FindComponent(BaseWeaponComponent));
			if (!weaponComp)
				continue;
			if (weaponComp.GetCurrentMagazine())
			{
				magazinewell = weaponComp.GetCurrentMagazine();
			}
			muzzleId = SCR_WeaponLib.GetNextMuzzleID(weaponComp);
			if (weaponComp && muzzleId != -1)
			{
				lowAmmo = CombatComp.EvaluateLowAmmo(weaponComp, muzzleId);
				SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(FindComponent(SCR_InventoryStorageManagerComponent));
				if (inv)
				{
					if (!inv.FindStorageForItem(magazinewell.GetOwner()))
					{
						lowAmmo = false;
					}
				}
				ammount = 2;
			}
		} 
		return lowAmmo;
	}
}