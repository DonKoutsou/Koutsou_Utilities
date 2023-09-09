class SCR_AIGetNeed : AITaskScripted
{

	protected static const string ENTITY_PORT = "Entity";
	protected static const string OUT_NEED_PORT = "OutNeed";
	protected static const string OUT_NEED_AMMOUNT_PORT = "OutNeedAmmount";
	override bool VisibleInPalette() { return true; }
	IEntity Char;
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		
		GetVariableIn(ENTITY_PORT, Char);
		if (!Char)
			Char = owner.GetControlledEntity();
		int ammount;
		ERequestRewardItemDesctiptor need = GetNeed(ammount);
		if (need)
		{
			SetVariableOut(OUT_NEED_PORT, need);
			SetVariableOut(OUT_NEED_AMMOUNT_PORT, ammount);
			return ENodeResult.SUCCESS;
		}
		return ENodeResult.FAIL;
	}
	protected static ref TStringArray s_aVarsIn = {ENTITY_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = {OUT_NEED_PORT, OUT_NEED_AMMOUNT_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	ERequestRewardItemDesctiptor GetNeed(out int ammount)
	{
		map <ERequestRewardItemDesctiptor, int> descmap = new map <ERequestRewardItemDesctiptor, int>();
		int severity
		if (CheckIfMissingBandages(ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.BANDAGE;
			descmap.Insert(ERequestRewardItemDesctiptor.BANDAGE, severity);
		}
		if (CheckIfMissingAmmo(ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.AMMO;
			descmap.Insert(ERequestRewardItemDesctiptor.AMMO, severity);
		}
		if (CheckIfMissingArmor(ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.ARMOR;
			descmap.Insert(ERequestRewardItemDesctiptor.ARMOR, severity);
		}
		if (CheckIfMissingHelmet(ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.HELMET;
			descmap.Insert(ERequestRewardItemDesctiptor.HELMET , severity);
		}
		if (CheckIfMissingBackpack(ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.BACKPACK;
			descmap.Insert(ERequestRewardItemDesctiptor.BACKPACK, severity);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.DRINK, ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.DRINK;
			descmap.Insert( ERequestRewardItemDesctiptor.DRINK, severity);
		}
		if (CheckIfMissingLBS(ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.LOAD_BEARING_SYSTEM;
			descmap.Insert(ERequestRewardItemDesctiptor.LOAD_BEARING_SYSTEM, severity);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.FLASHLIGHT, ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.FLASHLIGHT;
			descmap.Insert(ERequestRewardItemDesctiptor.FLASHLIGHT, severity);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.COMPASS, ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.COMPASS;
			descmap.Insert(ERequestRewardItemDesctiptor.COMPASS, severity);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.BINOCULARS, ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.BINOCULARS;
			descmap.Insert(ERequestRewardItemDesctiptor.BINOCULARS, severity);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.MAP, ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.MAP;
			descmap.Insert(ERequestRewardItemDesctiptor.MAP, severity);
		}
		if (CheckIfMissingItemOfType(ERequestRewardItemDesctiptor.EXPLOSIVE, ammount, severity))
		{
			if (severity > 4)
				return ERequestRewardItemDesctiptor.EXPLOSIVE;
			descmap.Insert(ERequestRewardItemDesctiptor.EXPLOSIVE, severity);
		}
		return null;
	}
	
	bool CheckIfMissingBandages(out int ammount, out int severity)
	{
		bool missingbandages;
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(Char.FindComponent(SCR_InventoryStorageManagerComponent));
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
		
		RequestCatalog.GetRequestItems(Desc, entries);
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(Char.FindComponent(SCR_InventoryStorageManagerComponent));
		
		array <IEntity> items = {};
		
		foreach (SCR_EntityCatalogEntry entry : entries)
		{
			SCR_PrefabNamePredicate pred = new SCR_PrefabNamePredicate();
			pred.prefabName = entry.GetPrefab();
			inventory.FindItems(items, pred);
		}
		if (items.Count() < 1)
		{
			missingItem == true;
			ammount = 1;
		}
		return missingItem;
	}
	
	bool CheckIfMissingHelmet(out int ammount, out int severity)
	{
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(Char.FindComponent(EquipedLoadoutStorageComponent));
		if (!loadoutStorage)
			return false;

		IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
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
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(Char.FindComponent(EquipedLoadoutStorageComponent));
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
	
	bool CheckIfMissingLBS(out int ammount, out int severity)
	{
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(Char.FindComponent(EquipedLoadoutStorageComponent));
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
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(Char.FindComponent(EquipedLoadoutStorageComponent));
		if (!loadoutStorage)
			return false;

		IEntity Vest = loadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
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
	bool CheckIfMissingAmmo(out int ammount, out int severity)
	{
		SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(Char.FindComponent(SCR_AICombatComponent));
		if (!CombatComp)
			return false;
		CharacterWeaponManagerComponent WeaponMan = CharacterWeaponManagerComponent.Cast(Char.FindComponent(CharacterWeaponManagerComponent));
		if (!WeaponMan)
			return false;
		
		BaseWeaponComponent weaponComp;
		BaseMagazineComponent magazineComp;
		int muzzleId;
		
		weaponComp = WeaponMan.GetCurrentWeapon();
		if (!weaponComp)
			return false;
		
		if (weaponComp)
		{
			if (weaponComp.GetCurrentMagazine())
			{
				magazineComp = weaponComp.GetCurrentMagazine();
			}
			muzzleId = SCR_WeaponLib.GetNextMuzzleID(weaponComp);
		}
		
		bool lowAmmo;
		
		if (weaponComp && muzzleId != -1)
		{
			lowAmmo = CombatComp.EvaluateLowAmmo(weaponComp, muzzleId);
			ammount = 2;
		}
		return lowAmmo;
	}
}