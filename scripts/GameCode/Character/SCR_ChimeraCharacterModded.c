modded class SCR_ChimeraCharacter
{
	[Attribute("0", desc : "Marks AI as important. Important characters are put into a different array than normal character in the CharacterHolder in request manager")]
	bool IsImportantCharacter;
	
	[Attribute(desc: "Needs that will be checked on this character", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ERequestRewardItemDesctiptor))]
	ref array <int> a_needstocheck;
	
	int needcheck;
	
	//needs that can be fulfilled
	ref array <int> a_needs;
	//needs that are too expensive to be fullfilled
	ref array <int> a_Unaffordableneeds;
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		//Initialise all arrays
		super.EOnInit(owner);
		if (!a_needs)
			a_needs = {};
		if (!a_Unaffordableneeds)
			a_Unaffordableneeds = {};
	}
	//get wallet of character. 
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
	//function called through BT using SCR_AIUpdateNeeds node. Itterates though needs and checks them. every time its called it checks another need.
	void UpdateNeeds()
	{
		int ammount;
		int severity;
		BaseMagazineComponent Mag;
		
		//checks if there are no needs configured
		if (a_needstocheck.IsEmpty())
		{
			Print("No needs configured in Chimera Character");
			return;
		}
		//Get the need to be checked on this cycle
		int need = a_needstocheck.Get(needcheck);
		
		//Check if need it still valid
		if (Checkneed(need, ammount, Mag))
		{
			//if yes see if character can afford it
			//Get characters money
			int money = GetWallet().GetCurrencyAmmount();
			int worth;
			//Get worth of need
			CheckNeedPrice(need, Mag, worth);
			//multiply based on ammount needed
			if (worth * ammount < money)
			{
				//if character has money for it he should set up a task for other AI to fulfill or the player
				if (!HasTask(this))
					CreateTask(this, need, ammount, Mag);
				//fill apropriate arrays with need
				if (!a_needs.Contains(need))
					a_needs.Insert(need);
				if (a_Unaffordableneeds.Contains(need))
					a_Unaffordableneeds.RemoveItem(need);
			}
			else
			{
				if (!IsLookingforWork() && !IsWorking())
					LookForWork();
				if (!a_Unaffordableneeds.Contains(need))
					a_Unaffordableneeds.Insert(need);
				if (a_needs.Contains(need))
					a_needs.RemoveItem(need);
				if (HasTaskForNeed(this, need))
					CompleteTasks(this, need);
			}
		}
		//if need is not valid anymore but we still have it in an array we need to make sure to get rid of it
		else if (a_needs.Contains(need) || a_Unaffordableneeds.Contains(need))
		{
			if (HasTaskForNeed(this, need))
					CompleteTasks(this, need);
			if (a_needs.Contains(need))
					a_needs.RemoveItem(need);
			if (a_Unaffordableneeds.Contains(need))
					a_Unaffordableneeds.RemoveItem(need);
		}
		//progress index so next time function is called it will check for something else
		needcheck += 1;
		//if index progressed to much reset it
		if (needcheck > a_needstocheck.Count() - 1)
			needcheck = 0;
		return;
	}
	bool IsLookingforWork()
	{
		return SP_RequestManagerComponent.CharIsPickingTask(this);
	}
	bool IsWorking()
	{
		return SP_RequestManagerComponent.GetassignedTaskCount(this) > 0;
	}
	bool IsFollowing()
	{
		return SP_RequestManagerComponent.CharFollowingAnybody(this);
	}
	void LookForWork()
	{
		if (IsImportantCharacter)
		{
			GetWallet().SpawnCurrency(20);
			return;
		}
		if (IsFollowing())
			return;	
		
		ChimeraWorld world = GetWorld();
		array <IEntity> chars = {};
		if (world.GetTagManager().GetTagsInRange(chars, GetOrigin(), 200, ETagCategory.NameTag))
		{
			for (int i; i < chars.Count(); i++;)
			{
				array <ref SP_Task> tasks = {};
				SP_RequestManagerComponent.GetCharOwnedTasks(chars.Get(i), tasks);
				if (tasks.IsEmpty())
					continue;
				SP_Task mytask = tasks.GetRandomElement();
				if (!mytask)
					continue;
				
				SP_Task sample = SP_RequestManagerComponent.GetTaskSample(mytask.GetClassName());
				if (!sample || !sample.m_bAssignable)
					continue;
				if (mytask.GetTarget() == this)
					continue;
				if (mytask.IsReserved())
					continue;
				if (mytask.GetState() == ETaskState.ASSIGNED)
					continue;
				if (mytask.CanBeAssigned(chars.Get(i), this) == false)
					continue;
				if (mytask.GetTimeLimit() < 3 && mytask.GetTimeLimit() != -1)
					continue;
				AIControlComponent comp = AIControlComponent.Cast(FindComponent(AIControlComponent));
				if (!comp)
					return;
				AIAgent agent = comp.GetAIAgent();
				if (!agent)
					return;
				SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
				if (!utility)
					return;
				SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
				if (!group)
					return;
				/*if (group.GetAgentsCount() > 1)
				{
					group.RemoveAgent(agent);
					Resource groupbase = Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et");
					EntitySpawnParams myparams = EntitySpawnParams();
					myparams.TransformMode = ETransformMode.WORLD;
					myparams.Transform[3] = Assignee.GetOrigin();
					SCR_AIGroup newgroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupbase, GetGame().GetWorld(), myparams));
					newgroup.AddAgent(agent);
					newgroup.AddWaypoint(group.GetCurrentWaypoint());
				}
				//else
					//group.CompleteWaypoint(group.GetCurrentWaypoint());*/
				SCR_AITaskPickupBehavior action = new SCR_AITaskPickupBehavior(utility, null, mytask);
				utility.AddAction(action);
				mytask.SetReserved(this);
				return;
			}
			
			
		}
	}
	//function used to check if character has task for this need and if yes cancel it. Used when a need is fulfilled
	void CompleteTasks(IEntity Owner, ERequestRewardItemDesctiptor need)
	{
		//task array
		array <ref SP_Task> tasks = {};
		
		//get tasks of character
		SP_RequestManagerComponent.GetCharTasksOfSameType( Owner , tasks , SP_RetrieveTask );
		
		//if no tasks on character return
		if ( tasks.IsEmpty( ) )
			return;
		
		//itterate through them and if need matches, cancel it and return
		foreach ( SP_Task task : tasks )
		{
			SP_RetrieveTask rtask = SP_RetrieveTask.Cast( task );
			if ( ! rtask )
				continue;
			if ( rtask.m_requestitemdescriptor == need )
			{
				task.CancelTask();
				return;
			}
		}
	}
	//Gets all needs that the UpdateNeeds function has gathered up until now
	void GetAllNeeds(out array <ERequestRewardItemDesctiptor> Needs)
	{
		Needs.Copy(a_needs);
		return;
	}
	// get a random need
	ERequestRewardItemDesctiptor GetNeed(out int ammount, out BaseMagazineComponent Mag = null)
	{
		if (a_needs.IsEmpty())
			return null;
		ERequestRewardItemDesctiptor need = a_needs.GetRandomElement();
		Checkneed(need, ammount, Mag);
		return need;

	}
	//Function used to check a price of a need to decide if able to fulfill it
	void CheckNeedPrice(ERequestRewardItemDesctiptor need, BaseMagazineComponent mag, out int price)
	{
		//Get request catalog
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType( EEntityCatalogType.REQUEST );
		
		//Get items matching my need
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(need, mag, Mylist);
		
		if (Mylist.IsEmpty())
			return;
		
		//Return price of one of them //Need to im
		for (int i; i < Mylist.Count(); i++;)
		{
			int pr = RequestCatalog.GetWorthOfItem(Mylist.Get(i).GetPrefab());
			if (!price)
				price = pr;
			if (pr < price)
				price = pr;
		}
	}
	//function used to check if a character has already created a task for the need
	bool HasTaskForNeed(SCR_ChimeraCharacter char, ERequestRewardItemDesctiptor Need)
	{
		//Get all of characters tasks
		array < ref SP_Task > tasks = {};
		SP_RequestManagerComponent.GetCharTasksOfSameType(char, tasks, SP_RetrieveTask);
		
		// if no tasks then return false
		if (tasks.IsEmpty())
			return false;
		else
			return true;
		//itterate through tasks and if any of the retrieve tasks has the need return true
		foreach (SP_Task task : tasks)
		{
			SP_RetrieveTask rtask = SP_RetrieveTask.Cast(task);
			if (!rtask)
				continue;
			if (rtask.m_requestitemdescriptor == Need)
			{
				return true;
			}
		}
		//if none matched return false
		return false;
	}
	bool HasTask(SCR_ChimeraCharacter char)
	{
		//Get all of characters tasks
		array < ref SP_Task > tasks = {};
		SP_RequestManagerComponent.GetCharTasksOfSameType(char, tasks, SP_RetrieveTask);
		
		// if no tasks then return false
		if (tasks.IsEmpty())
			return false;
		else
			return true;
	}
	//function used to create a SP_RetrieveTask using a need
	bool CreateTask(SCR_ChimeraCharacter char, ERequestRewardItemDesctiptor Need, int ammount, BaseMagazineComponent Mag = null)
	{
		SP_RequestManagerComponent Requestman = SP_RequestManagerComponent.GetInstance();
		return Requestman.CreateCustomRetrieveTask(char, Need, ammount, Mag);
	}
	//checks a specific need. If its still valid it will return true. Also used to retrieve ammount needed and Magazine if need is ammo
	bool Checkneed(ERequestRewardItemDesctiptor need, out int ammount, out BaseMagazineComponent Mag = null)
	{
		int severity;
		//BANDAGES
		if (need == ERequestRewardItemDesctiptor.BANDAGE)
		{
			if (CheckIfMissingBandages(ammount, severity))
			{
				return true;
			}
		}
		//WEAPON
		else if (need == ERequestRewardItemDesctiptor.WEAPON)
		{
			if (CheckIfMissingWeapon(ammount, severity))
			{
				return true;
			}
		}
		//AMMO
		else if (need == ERequestRewardItemDesctiptor.AMMO)
		{
			if (CheckIfMissingAmmo(ammount, severity, Mag))
			{
				return true;
			}
		}
		//ARMOR
		else if (need == ERequestRewardItemDesctiptor.ARMOR)
		{
			if (CheckIfMissingArmor(ammount, severity))
			{
				return true;
			}
		}
		//HELMET
		else if (need == ERequestRewardItemDesctiptor.HELMET)
		{
			if (CheckIfMissingHelmet(ammount, severity))
			{
				return true;
			}
		}
		//BACKPACK
		else if (need == ERequestRewardItemDesctiptor.BACKPACK)
		{
			if (CheckIfMissingBackpack(ammount, severity))
			{
				return true;
			}
		}
		//LOAD_BEARING_SYSTEM
		else if (need == ERequestRewardItemDesctiptor.LOAD_BEARING_SYSTEM)
		{
			if (CheckIfMissingLBS(ammount, severity))
			{
				return true;
			}
		}
		//BINOCULARS
		else if (need == ERequestRewardItemDesctiptor.BINOCULARS)
		{
			if (CheckIfMissingBinoculars(ammount, severity))
			{
				return true;
			}
		}
		//EXPLOSIVE
		else if (need == ERequestRewardItemDesctiptor.EXPLOSIVE)
		{
			if (CheckIfMissingExplosives(ammount, severity))
			{
				return true;
			}
		}
		//multipurpose
		else if (CheckIfMissingItemOfType(need, ammount, severity))
		{
			return true;
		}
		return false;
	}
	//NEED CHECKING FUNCTION BELLOW HERE
	//---------------------------------------------------------------------------------------------------------------------------------------------------------//
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
			inventory.FindItems(items, pred, EStoragePurpose.PURPOSE_ANY);
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
	bool CheckIfMissingLBS(out int ammount, out int severity)
	{
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!loadoutStorage)
			return false;
				
		IEntity LBS = loadoutStorage.GetClothFromArea(LoadoutVestArea);
		if (!LBS)
		{
			array<typename> blockedSlots = {};
			loadoutStorage.GetBlockedSlots(blockedSlots);
			if (!blockedSlots.IsEmpty())
			{
				if (blockedSlots.Contains(LoadoutVestArea))
				{
					return false;
				}
			}
			
			ammount = 1;
			return true;
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
			if (weaponComp && muzzleId != -1 && magazinewell)
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