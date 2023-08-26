//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_RetrieveTask: SP_Task
{
	//----------------------------------------//
	//amount of requested items
	int	m_iRequestedAmount;
	//----------------------------------------//
	//Using rewards from entity catalog in game mode
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(SCR_EArsenalItemType))]
	SCR_EArsenalItemType	m_requestitemtype;
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(SCR_EArsenalItemMode))]
	SCR_EArsenalItemMode	m_requestitemmode;
	
	ref array <ResourceName>	rewards;
	//----------------------------------------//
	ref array <IEntity> m_aRetrievedItems;
	//------------------------------------------------------------------------------------------------------------//
	override bool Init()
	{
		InheritFromSample();
		if (!m_eTaskOwner)
		{
			//first look for owner cause targer is usually derived from owner faction/location etc...
			if (!FindOwner(m_eTaskOwner))
			{
				return false;
			}
			//-------------------------------------------------//
			//function to fill to check ckaracter
			if(!CheckOwner())
			{
				return false;
			}
		}
		if (!SetupRequestTypenMode())
		{
			return false;
		}
		CreateDescritions();
		AddOwnerInvokers();
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	override void CreateDescritions()
	{
		string OName;
		string OLoc;
		GetInfo(OName, OLoc);
		string itemdesc = typename.EnumToString(SCR_EArsenalItemType, m_requestitemtype) + " " + typename.EnumToString(SCR_EArsenalItemMode, m_requestitemmode);
		itemdesc.ToLower();
		m_sTaskDesc = string.Format("%1 is looking for %2 %3. %1 is on %4, go meet him if you can help him.", OName, m_iRequestedAmount.ToString(), itemdesc, OLoc);
		m_sTaskDiag = string.Format("I am looking for someone to bring me %1 %2.", m_iRequestedAmount.ToString(), itemdesc);
		m_sTaskTitle = string.Format("Retrieve %1 %2 for %3 ",m_iRequestedAmount.ToString(), itemdesc, OName);
		m_sTaskCompletiontext = "Thanks for retrieving the items i asked for %1. Here is the reward, hope is suficient";
		m_sAcceptTest = string.Format("I'll get you the %1.", itemdesc);
		m_sacttext = string.Format("I've brought the %1 you asked.", itemdesc);
	};
	//------------------------------------------------------------------------------------------------------------//
	bool SetupRequestTypenMode()
	{
		if (e_RewardLabel && m_requestitemtype && m_requestitemmode)
			return true;
		int index = Math.RandomInt(0, 12);
		if (index == 0)
			{
				m_requestitemtype = SCR_EArsenalItemType.HEAL;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 1)
			{
				m_requestitemtype = SCR_EArsenalItemType.FOOD;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 2)
			{
				m_requestitemtype = SCR_EArsenalItemType.DRINK;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 3)
			{
				m_requestitemtype = SCR_EArsenalItemType.EXPLOSIVES;
				m_requestitemmode = SCR_EArsenalItemMode.WEAPON;
				m_iRequestedAmount = Math.RandomInt(1, 10);
				if (m_iRequestedAmount < 5)
					e_RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
				return true;
			}
		if (index == 4)
			{
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(m_eTaskOwner.FindComponent(EquipedLoadoutStorageComponent));
				if (!loadoutStorage)
					return false;

				IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
				if (Helmet)
				{
					EntityPrefabData prefabData = Helmet.GetPrefabData();
					ResourceName prefabName = prefabData.GetPrefabName();
					SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
					SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
					SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
					if(entry)
					{
						return false;
					}
				}
				m_requestitemtype = SCR_EArsenalItemType.HEADWEAR;
				m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
				m_iRequestedAmount = 1;
				return true;
			}
		if (index == 5)
			{
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(m_eTaskOwner.FindComponent(EquipedLoadoutStorageComponent));
				if (!loadoutStorage)
					return false;
				
				IEntity Backpack = loadoutStorage.GetClothFromArea(LoadoutBackpackArea);
				if (Backpack)
				{
					return false;
				}
				m_requestitemtype = SCR_EArsenalItemType.BACKPACK;
				m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
				m_iRequestedAmount = 1;
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 6)
			{
				m_requestitemtype = SCR_EArsenalItemType.FLASHLIGHT;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = 1;
			e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 7)
			{
				m_requestitemtype = SCR_EArsenalItemType.MAP;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 8)
			{
				m_requestitemtype = SCR_EArsenalItemType.COMPASS;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 9)
			{
				m_requestitemtype = SCR_EArsenalItemType.RADIO;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 10)
			{
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(m_eTaskOwner.FindComponent(EquipedLoadoutStorageComponent));
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
					if(entry)
					{
						return false;
					}
				}
				m_requestitemtype = SCR_EArsenalItemType.ARMOR;
				m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
				m_iRequestedAmount = 1;
				return true;
			}
		if (index == 11)
			{
				m_requestitemtype = SCR_EArsenalItemType.SLEEPING_PILLS;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = 1;
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	
	override void SpawnTaskMarker(IEntity Assignee)
	{
		Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		m_eTaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		m_TaskMarker.SetTitle(m_sTaskTitle);
		m_TaskMarker.SetDescription(m_sTaskDesc);
		m_TaskMarker.SetTarget(m_eTaskOwner);
		m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar != m_eTaskOwner)
		{
			return false;
		}
		if(!CharacterAssigned(Assignee))
		{
			return false;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemtype, m_requestitemmode);
		array <IEntity> FoundItems = new array <IEntity>();
		inv.FindItems(FoundItems, RequestPred);
		if (FoundItems.Count() >= m_iRequestedAmount)
		{
			int m_ifoundamount;
			foreach (IEntity item : FoundItems)
				{
					InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
					InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
					LoadoutSlotInfo eqslot = LoadoutSlotInfo.Cast(parentSlot);
					if(!eqslot)
						{
							m_ifoundamount += 1;
						}
				}
			if(m_ifoundamount >= m_iRequestedAmount)
			{
				return true;
			}
		}
		return false;			
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool CompleteTask(IEntity Assignee)
	{
		
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(Assignee.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemtype, m_requestitemmode);
		array <IEntity> FoundItems = new array <IEntity>();
		inv.FindItems(FoundItems, RequestPred);
		int amountleft = m_iRequestedAmount;
		if (FoundItems.Count() >= m_iRequestedAmount)
		{			
			m_aRetrievedItems = new ref array <IEntity>();
			foreach (IEntity item : FoundItems)
			{
				if (amountleft <= 0)
					break;
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				LoadoutSlotInfo eqslot = LoadoutSlotInfo.Cast(parentSlot);
				if(!eqslot)
				{
					inv.TryRemoveItemFromStorage(item,parentSlot.GetStorage());
					if(m_requestitemtype == SCR_EArsenalItemType.HEADWEAR)
					{
						Ownerinv.TryInsertItem(item);
						Ownerinv.EquipCloth(item);
					}
					else if(m_requestitemtype == SCR_EArsenalItemType.BACKPACK)
					{
						Ownerinv.EquipCloth(item);
					}
					else if(m_requestitemtype == SCR_EArsenalItemType.ARMOR)
					{
						Ownerinv.EquipCloth(item);
					}
					else
					{
						Ownerinv.TryInsertItem(item);
					}
					m_aRetrievedItems.Insert(item);
					//if (ItemBounty)
						//delete ItemBounty;
					amountleft -= 1;
				}
			}
			AssignReward();
			if (GiveReward(Assignee))
			{
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
				}
				e_State = ETaskState.COMPLETED;
				m_eCopletionist = Assignee;
				GetOnTaskFinished(this);
				SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
				dmgmn.GetOnDamageStateChanged().Remove(FailTask);
				SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
				return true;
				
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string OLoc)
	{
		if (!m_eTaskOwner)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
	};
	override bool AssignReward()
	{
		rewards = new ref array <ResourceName>();
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		SCR_EntityCatalog RewardCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Requestlist = new array<SCR_EntityCatalogEntry>();
		array<SCR_EntityCatalogEntry> Rewardlist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityList(Requestlist);
		if (m_aRetrievedItems.IsEmpty())
			return false;
		foreach (IEntity Item :m_aRetrievedItems)
		{
			EntityPrefabData prefabData = Item.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
			SCR_ArsenalItem arsenaldata = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
			m_iRewardAmount = m_iRewardAmount + arsenaldata.GetSupplyCost();
		}
		RewardCatalog.GetEntityList(Rewardlist);
		while (m_iRewardAmount > 0)
		{
			SCR_EntityCatalogEntry entry = Rewardlist.GetRandomElement();
			SCR_ArsenalItem arsenaldata = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
			if (arsenaldata.GetSupplyCost() <= m_iRewardAmount)
			{
				m_iRewardAmount -= arsenaldata.GetSupplyCost();
				rewards.Insert(entry.GetPrefab());
			}
		}
		/*if (!e_RewardLabel)
		{
			int index = Math.RandomInt(0,2);
			if(index == 0)
			{
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				//m_iRewardAmount = m_iRewardAmount * m_iRequestedAmount;
			}
			if(index == 1)
			{
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
				//m_iRewardAmount = 1;
			}
		}
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();*/
		return true;
		
	};
	override bool GiveReward(IEntity Target)
	{
		if (!rewards.IsEmpty())
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = m_eTaskOwner.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			int Movedamount;
			for (int j = 0; j < rewards.Count(); j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(Resource.Load(rewards[j]), GetGame().GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				TargetInv.TryInsertItem(Rewardlist[i]);
				Movedamount += 1;
			}
			string rewardstring;
			map <ResourceName, int> stringarray = new map <ResourceName, int>();
			for (int j = 0; j < rewards.Count(); j++)
			{
				if (!stringarray.Contains(rewards[j]))
					stringarray.Insert(rewards[j], 1);
				else
					stringarray.Set(rewards[j], stringarray.Get(rewards[j]) + 1);
			}
			for (int j = 0; j < stringarray.Count(); j++)
			{
				string itemstring = FilePath.StripPath(stringarray.GetKey(j));
				itemstring.ToLower();
				rewardstring = rewardstring + stringarray.Get(stringarray.GetKey(j)) + " " + itemstring.Substring(0, itemstring.Length() - 3) + ", ";
			}
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_RetrieveTask;};
};
//------------------------------------------------------------------------------------------------------------//
class SP_RequestPredicate : InventorySearchPredicate
{
	SCR_EArsenalItemType	m_requestitemtype;
	SCR_EArsenalItemMode	m_requestitemmode;
	//------------------------------------------------------------------------------------------------------------//
	void SP_RequestPredicate(SCR_EArsenalItemType type, SCR_EArsenalItemMode mode){m_requestitemtype = type; m_requestitemmode = mode;};
	//------------------------------------------------------------------------------------------------------------//
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		EntityPrefabData prefabData = item.GetPrefabData();
		ResourceName prefabName = prefabData.GetPrefabName();
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
		if(!entry)
		{
			return false;
		}
		SCR_ArsenalItem arsenaldata = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
		if(m_requestitemtype && m_requestitemmode)
		{
			if (arsenaldata.GetItemType() == m_requestitemtype && arsenaldata.GetItemMode() == m_requestitemmode)
			{
				return true;
			}
		}
		else if(m_requestitemtype && !m_requestitemmode)
		{
			if (arsenaldata.GetItemType() == m_requestitemtype)
			{
				return true;
			}
		}
		else if(!m_requestitemtype && m_requestitemmode)
		{
			if (arsenaldata.GetItemMode() == m_requestitemmode)
			{
				return true;
			}
		}
		return false;
	}
	
};
//------------------------------------------------------------------------------------------------------------//
modded enum EEditableEntityLabel
{
	ITEMTYPE_CURRENCY = 85,
	ITEMTYPE_STASH = 86,
	ITEMTYPE_SLEEPINGPILLS = 87,
}