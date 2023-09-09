//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_RetrieveTask: SP_Task
{
	//----------------------------------------//
	//amount of requested items
	int	m_iRequestedAmount;
	//----------------------------------------//
	//Using rewards from entity catalog in game mode
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ERequestRewardItemDesctiptor))]
	ERequestRewardItemDesctiptor	m_requestitemdescriptor;
	
	ref array <IEntity>	rewards;
	//----------------------------------------//
	ref array <IEntity> m_aRetrievedItems;
	//incase task is for ammo
	BaseMagazineComponent Mag;
	//------------------------------------------------------------------------------------------------------------//
	ERequestRewardItemDesctiptor GetRequestDescriptor()
	{
		return m_requestitemdescriptor;
	}
	int GetRequestAmmount()
	{
		return m_iRequestedAmount;
	}
	override bool CanBeAssigned(IEntity TalkingChar, IEntity Assignee)
	{
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(Assignee.FindComponent(SCR_InventoryStorageManagerComponent));
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemdescriptor);
		array <IEntity> FoundItems = new array <IEntity>();
		inv.FindItems(FoundItems, RequestPred);
		if (FoundItems.Count() > m_iRequestedAmount)
		{
			return true;
		}
		return false;
	}
	override bool AssignCharacter(IEntity Character)
	{
		
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		if (!comp)
			return false;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return false;
		if (!super.AssignCharacter(Character))
			return false;
		if (SCR_EntityHelper.GetPlayer() != Character)
		{
			if (CompleteTask(Character))
				return true;
			else
				UnAssignCharacter();
			
		}
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return false;
		SCR_AIExecuteRetrieveTaskBehavior action = new SCR_AIExecuteRetrieveTaskBehavior(utility, null, this);
		utility.AddAction(action);
		//if player throw popup
		AddAssigneeInvokers();
		return true;
	}
	override bool AssignOwner()
	{
		return false;
	}
	override bool Init()
	{
		if (!m_bPartOfChain)
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
		if (!SetupRequestTypenMode(m_eTaskOwner))
		{
			return false;
		}
		SetTimeLimit();
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
		string itemdesc
		if (m_requestitemdescriptor == ERequestRewardItemDesctiptor.AMMO)
			itemdesc = Mag.GetMagazineWell().ClassName() + " " + typename.EnumToString(ERequestRewardItemDesctiptor, m_requestitemdescriptor);
		else
			itemdesc = typename.EnumToString(ERequestRewardItemDesctiptor, m_requestitemdescriptor);
		itemdesc.ToLower();
		m_sTaskDesc = string.Format("%1 is looking for %2 %3. %1 is on %4, go meet him if you can help him.", OName, m_iRequestedAmount.ToString(), itemdesc, OLoc);
		m_sTaskDiag = string.Format("I am looking for someone to bring me %1 %2.", m_iRequestedAmount.ToString(), itemdesc);
		m_sTaskTitle = string.Format("Retrieve %1 %2 for %3 ",m_iRequestedAmount.ToString(), itemdesc, OName);
		m_sTaskCompletiontext = "Thanks for retrieving the items i asked for %1. Here is the reward, hope is suficient";
		m_sAcceptTest = string.Format("I'll get you the %1.", itemdesc);
		m_sacttext = string.Format("I've brought the %1 you asked.", itemdesc);
	};
	//------------------------------------------------------------------------------------------------------------//
	bool SetupRequestTypenMode(IEntity Owner)
	{
		if (m_requestitemdescriptor)
			return true;
		SCR_ChimeraCharacter ChimeraChar = SCR_ChimeraCharacter.Cast(Owner);
		int ammount;
		
		m_requestitemdescriptor = ChimeraChar.GetNeed(ammount, Mag);
		if (!m_requestitemdescriptor)
			return false;
		
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		int money = ChimeraChar.GetWallet().GetCurrencyAmmount();
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(m_requestitemdescriptor, Mylist);
		SCR_EntityCatalogEntry entry;
		if (m_requestitemdescriptor == ERequestRewardItemDesctiptor.AMMO)
		{
			
			foreach (SCR_EntityCatalogEntry disentry : Mylist)
			{
				SP_RequestAmmoData data = SP_RequestAmmoData.Cast(disentry.GetEntityDataOfType(SP_RequestAmmoData));
				if (data.GetMagType().ClassName() == Mag.GetMagazineWell().ClassName())
				{
					entry = disentry;
					break;
				}
			}
			
		}
		else
			entry = Mylist.GetRandomElement();
		if (!entry)
				return false;
		int worth = RequestCatalog.GetWorthOfItem(entry.GetPrefab());
		while (worth * ammount > money)
		{
			ammount -= 1;
			if (ammount == 0)
				return false;
		}
		m_iRequestedAmount = ammount;
		return true;
		
		/*
		int index = Math.RandomInt(0, 22);
		if (index == 0)
			{
				m_requestitemdescriptor = ERequestRewardItemDesctiptor.BANDAGE;
				return true;
			}
		if (index == 1)
			{
				m_iRequestedAmount = Math.RandomInt(1, 3);
				return true;
			}
		if (index == 2)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.DRINK;
				//m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 3)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.EXPLOSIVES;
				//m_requestitemmode = SCR_EArsenalItemMode.WEAPON;
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
				m_requestitemdescriptor = SCR_EArsenalItemType.HEADWEAR;
				//m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
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
				m_requestitemdescriptor = SCR_EArsenalItemType.BACKPACK;
				//m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
				m_iRequestedAmount = 1;
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 6)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.FLASHLIGHT;
				//m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = 1;
			e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 7)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.MAP;
				//m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 8)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.COMPASS;
				//m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}
		if (index == 9)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.RADIO;
				//m_requestitemmode = SCR_EArsenalItemMode.GADGET;
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
				m_requestitemdescriptor = SCR_EArsenalItemType.ARMOR;
				//m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
				m_iRequestedAmount = 1;
				return true;
			}
		if (index == 11)
			{
				m_requestitemdescriptor = SCR_EArsenalItemType.SLEEPING_PILLS;
				//m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = 1;
				e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
				return true;
			}*/
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
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemdescriptor);
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
					if (m_requestitemdescriptor == ERequestRewardItemDesctiptor.AMMO)
					{
						BaseMagazineComponent comp = BaseMagazineComponent.Cast(item.FindComponent(BaseMagazineComponent));
						if (!comp)
							continue;
						if (comp.GetMagazineWell().ClassName() == Mag.GetMagazineWell().ClassName())
						{
							m_ifoundamount += 1;
						}
					}
					else
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
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemdescriptor);
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
				if (m_requestitemdescriptor == ERequestRewardItemDesctiptor.AMMO)
				{
					BaseMagazineComponent comp = BaseMagazineComponent.Cast(item.FindComponent(BaseMagazineComponent));
					if (!comp)
						continue;
					if (comp.GetMagazineWell().ClassName() != Mag.GetMagazineWell().ClassName())
					{
						continue;
					}
				}
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				LoadoutSlotInfo eqslot = LoadoutSlotInfo.Cast(parentSlot);
				if(!eqslot)
				{
					inv.TryRemoveItemFromStorage(item,parentSlot.GetStorage());
					if(m_requestitemdescriptor == ERequestRewardItemDesctiptor.HELMET)
					{
						Ownerinv.TryInsertItem(item);
						Ownerinv.EquipCloth(item);
					}
					else if(m_requestitemdescriptor == ERequestRewardItemDesctiptor.BACKPACK)
					{
						Ownerinv.EquipCloth(item);
					}
					else if(m_requestitemdescriptor == ERequestRewardItemDesctiptor.ARMOR)
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
			if (amountleft != 0)
				return false;
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
				if (SCR_EntityHelper.GetPlayer() == Assignee)
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
		rewards = new ref array <IEntity>();
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		array<SCR_EntityCatalogEntry> Requestlist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityList(Requestlist);
		if (m_aRetrievedItems.IsEmpty())
			return false;
		foreach (IEntity Item :m_aRetrievedItems)
		{
			EntityPrefabData prefabData = Item.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
			SP_RequestData requestdata = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
			m_iRewardAmount = m_iRewardAmount + requestdata.GetWorth();
		}
		while (m_iRewardAmount > 0)
		{
			
			SCR_EntityCatalogEntry entry = Requestlist.GetRandomElement();
			SP_RequestData requestdata = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
			if (e_RewardLabel == ERequestRewardItemDesctiptor.CURRENCY)
			{
				SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(m_eTaskOwner);
				WalletEntity wallet = Char.GetWallet();
				if (wallet.GetCurrencyAmmount() > m_iRewardAmount)
				{
					array <IEntity> curr = {};
					wallet.GetCurrency(curr);
					foreach (IEntity mon : curr)
					{
						if (!m_iRewardAmount)
							break;
						rewards.Insert(mon);
						m_iRewardAmount -= 1;
					}
				}
			}
			//else if (requestdata.GetWorth() <= m_iRewardAmount)
			//{
			//	m_iRewardAmount -= requestdata.GetWorth();
			//	rewards.Insert(entry.GetPrefab());
			//}
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
		m_bHasReward = true;
		return true;
		
	};
	override bool GiveReward(IEntity Target)
	{
		if (!m_bHasReward)
		{
			return true;
		}
		if (!rewards.IsEmpty())
		{
			
			if (e_RewardLabel != ERequestRewardItemDesctiptor.CURRENCY)
			{
				InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
				array<IEntity> Rewardlist = new array<IEntity>();
				int Movedamount;
				for (int j = 0; j < rewards.Count(); j++)
					Rewardlist.Insert(rewards[j]);
				for (int i, count = Rewardlist.Count(); i < count; i++)
				{
					TargetInv.TryInsertItem(Rewardlist[i]);
					Movedamount += 1;
				}
			}
			else
			{
				SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(Target);
				SCR_ChimeraCharacter OChar = SCR_ChimeraCharacter.Cast(m_eTaskOwner);
				WalletEntity wallet = Char.GetWallet();
				WalletEntity Owallet = OChar.GetWallet();
				return wallet.TakeCurrency(Owallet, rewards.Count());
			}
			string rewardstring;
			map <ResourceName, int> stringarray = new map <ResourceName, int>();
			for (int j = 0; j < rewards.Count(); j++)
			{
				if (!stringarray.Contains(rewards[j].GetPrefabData().GetPrefabName()))
					stringarray.Insert(rewards[j].GetPrefabData().GetPrefabName(), 1);
				else
					stringarray.Set(rewards[j].GetPrefabData().GetPrefabName(), stringarray.Get(rewards[j].GetPrefabData().GetPrefabName()) + 1);
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
	override void InheritFromSample()
	{
		super.InheritFromSample();
		SP_RetrieveTask tasksample = SP_RetrieveTask.Cast(SP_RequestManagerComponent.GetTaskSample(GetClassName()));
		if(!tasksample)
		{
			return;
		}
		m_requestitemdescriptor = tasksample.m_requestitemdescriptor;
	}
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_RetrieveTask;};
};
//------------------------------------------------------------------------------------------------------------//
class SP_RequestPredicate : InventorySearchPredicate
{
	ERequestRewardItemDesctiptor	m_requestitemdescriptor;
	//------------------------------------------------------------------------------------------------------------//
	//Constructor
	void SP_RequestPredicate(ERequestRewardItemDesctiptor type){m_requestitemdescriptor = type;};
	//------------------------------------------------------------------------------------------------------------//
	//Check if request data exist and match ItemDesriptor
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
		if (m_requestitemdescriptor != ERequestRewardItemDesctiptor.AMMO)
		{
			SP_RequestData Requestdata = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
			if(Requestdata)
			{
				if (Requestdata.GetRequestDescriptor() == m_requestitemdescriptor)
				{
					return true;
				}
			}
		}
		else
		{
			SP_RequestAmmoData Requestammodata = SP_RequestAmmoData.Cast(entry.GetEntityDataOfType(SP_RequestAmmoData));
			if(Requestammodata)
			{
				if (Requestammodata.GetRequestDescriptor() == m_requestitemdescriptor)
				{
					return true;
				}
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