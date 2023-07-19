//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_RetrieveTask: SP_Task
{
	//----------------------------------------//
	IEntity	ItemBounty;
	//----------------------------------------//
	int	m_iRequestedAmount;
	//----------------------------------------//
	SCR_EArsenalItemType	m_requestitemtype;
	SCR_EArsenalItemMode	m_requestitemmode;
	//------------------------------------------------------------------------------------------------------------//
	override bool Init()
	{
		if (!FindOwner(TaskOwner))
		{
			return false;
		}
		if(!CheckCharacter(TaskOwner))
		{
			return false;
		}
		if (!SetupTaskEntity())
		{
			DeleteLeftovers();
			return false;
		}
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		CreateDescritions();
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
		if (!MyDirector)
		{
			return false;
		}
		if (!MyDirector.GetRandomUnit(Owner))
		{
			return false;
		}
		if(Owner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool SetupTaskEntity()
	{
		if (TaskOwner)
		{
			string OName;
			string OLoc;
			GetInfo(OName, OLoc);
			if (OName == " " || OLoc == " ")
			{
				return false;
			}
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = vector.Zero;
			Resource res = Resource.Load("{A99880B995D4588F}prefabs/Items/ItemBountyPaper.et");
			if (res)
			{
				ItemBounty = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
				if (inv.TryInsertItem(ItemBounty) == false)
				{
					return false;
				}
			}
			if(!SetupRequestTypenMode())
			{
				return false;
			}
			SP_ItemBountyComponent IBComp = SP_ItemBountyComponent.Cast(ItemBounty.FindComponent(	SP_ItemBountyComponent));
			IBComp.SetInfo(OName, m_requestitemtype, m_requestitemmode, EEditableEntityLabel.ITEMTYPE_ITEM, m_iRequestedAmount, OLoc);
			e_State = ETaskState.UNASSIGNED;
			return true;
		}
		return false;
	};
	override void CreateDescritions()
	{
		string OName;
		string OLoc;
		GetInfo(OName, OLoc);
		string itemdesc = typename.EnumToString(SCR_EArsenalItemType, m_requestitemtype) + " " + typename.EnumToString(SCR_EArsenalItemMode, m_requestitemmode);
		TaskDesc = string.Format("%1 is looking for %2 %3. Reward is %4 %5", OName, m_iRequestedAmount.ToString(), itemdesc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskDiag = string.Format("I'm looking for %1 %2. Come back to find me on %3. Reward is %4 %5", m_iRequestedAmount.ToString(), itemdesc, OLoc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskTitle = string.Format("Retrieve: bring %1 %2 to %3 ",m_iRequestedAmount.ToString(), itemdesc, OName);
	};
	//------------------------------------------------------------------------------------------------------------//
	bool SetupRequestTypenMode()
	{
		int index = Math.RandomInt(0, 10);
		if (index == 0)
			{
				m_requestitemtype = SCR_EArsenalItemType.HEAL;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				m_iRewardAmount = 2;
				return true;
			}
		if (index == 1)
			{
				m_requestitemtype = SCR_EArsenalItemType.FOOD;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				m_iRewardAmount = 3;
				return true;
			}
		if (index == 2)
			{
				m_requestitemtype = SCR_EArsenalItemType.DRINK;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				m_iRewardAmount = 2;
				return true;
			}
		if (index == 3)
			{
				m_requestitemtype = SCR_EArsenalItemType.EXPLOSIVES;
				m_requestitemmode = SCR_EArsenalItemMode.WEAPON;
				m_iRequestedAmount = Math.RandomInt(1, 10);
				m_iRewardAmount = 5;
				return true;
			}
		if (index == 4)
			{
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(TaskOwner.FindComponent(EquipedLoadoutStorageComponent));
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
				m_iRewardAmount = 10;
				return true;
			}
		if (index == 5)
			{
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(TaskOwner.FindComponent(EquipedLoadoutStorageComponent));
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
				m_iRewardAmount = 10;
				return true;
			}
		if (index == 6)
			{
				m_requestitemtype = SCR_EArsenalItemType.FLASHLIGHT;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = 1;
				m_iRewardAmount = 10;
				return true;
			}
		if (index == 7)
			{
				m_requestitemtype = SCR_EArsenalItemType.MAP;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = Math.RandomInt(1, 3);
				m_iRewardAmount = 2;
				return true;
			}
		if (index == 8)
			{
				m_requestitemtype = SCR_EArsenalItemType.COMPASS;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = 1;
				m_iRewardAmount = 5;
				return true;
			}
		if (index == 9)
			{
				m_requestitemtype = SCR_EArsenalItemType.RADIO;
				m_requestitemmode = SCR_EArsenalItemMode.GADGET;
				m_iRequestedAmount = 1;
				m_iRewardAmount = 10;
				return true;
			}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignReward()
	{
		EEditableEntityLabel RewardLabel;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			m_iRewardAmount = m_iRewardAmount * m_iRequestedAmount;
		}
		if(index == 1)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		return true;
	};
	override void SpawnTaskMarker()
	{
		Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		TaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		m_TaskMarker.SetTitle(TaskTitle);
		m_TaskMarker.SetDescription(TaskDesc);
		m_TaskMarker.SetTarget(TaskOwner);
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(a_TaskAssigned[0]);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar != TaskOwner)
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
			if(m_ifoundamount == m_iRequestedAmount)
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
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemtype, m_requestitemmode);
		array <IEntity> FoundItems = new array <IEntity>();
		inv.FindItems(FoundItems, RequestPred);
		if (FoundItems.Count() >= m_iRequestedAmount)
		{
			if (GiveReward(Assignee))
			{
				foreach (IEntity item : FoundItems)
				{
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
						else
						{
							Ownerinv.TryInsertItem(item);
						}

						delete ItemBounty;
					}
				}
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
				}
				e_State = ETaskState.COMPLETED;
				m_Copletionist = Assignee;
				SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
				reqman.OnTaskCompleted(this);
				return true;
				
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override void UpdateState()
	{
		SCR_CharacterDamageManagerComponent OwnerDmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (OwnerDmgComp.IsDestroyed())
		{
			if (m_TaskMarker)
			{
				m_TaskMarker.Fail(true);
			}
			e_State = ETaskState.FAILED;
			return;
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string OLoc)
	{
		if (!TaskOwner)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		AIControlComponent comp = AIControlComponent.Cast(TaskOwner.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
		OLoc = Director.GetCharacterLocation(TaskOwner);
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_RetrieveTask;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetItemBountyEnt(){return ItemBounty;};
	//------------------------------------------------------------------------------------------------------------//
	override void DeleteLeftovers()
	{
		if(ItemBounty)
		{
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(ItemBounty.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			if(parentSlot)
			{
				SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
				if(inv)
				{
					inv.TryRemoveItemFromStorage(ItemBounty,parentSlot.GetStorage());
					delete ItemBounty;
				}
			}
		}
		if(ItemBounty)
		{
			delete ItemBounty;
		}
	};
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
	ITEMTYPE_STASH = 86
}