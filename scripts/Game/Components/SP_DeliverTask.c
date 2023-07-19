//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_DeliverTask: SP_Task
{
	//----------------------------------------------------------------------------------//
	[Attribute(defvalue: "10", desc: "Reward amount if reward end up being currency")]
	int m_iRewardAverageAmount;
	//----------------------------------------------------------------------------------//
	//Package that needs to be delivered
	IEntity Package;
	//------------------------------------------------------------------------------------------------------------//
	//Delivery mission is looking for a random owner.
	override bool FindOwner(out IEntity Owner)
	{
		SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
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
	//then look for a target with same faction
	override bool FindTarget(out IEntity Target)
	{
		AIControlComponent comp = AIControlComponent.Cast(GetOwner().FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector MyDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SP_AIDirector NewDirector;
		//-----------------------------------------------------------------//
		FactionManager factionManager = FactionManager.Cast(GetGame().GetFactionManager());
		string keyunused;
		Faction directorFaction = MyDirector.GetMajorityHolder(keyunused);
		if (!directorFaction)
			return false;
		FactionKey key = directorFaction.GetFactionKey();
		SCR_Faction myFaction = SCR_Faction.Cast(factionManager.GetFactionByKey(key));
		if (!MyDirector.GetDirectorOccupiedByFriendly(myFaction, NewDirector))
		{
			return false;
		}
		if(MyDirector == NewDirector)
		{
			return false;
		}
		if (!NewDirector.GetRandomUnitByFKey(key, Target))
		{
			return false;
		}
		if (Target == GetOwner())
		{
			return false;
		}
		if(Target)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Setup delivery package
	override bool SetupTaskEntity()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName,OLoc, DLoc);
		if (OName == " " || DName == " " || DLoc == " ")
		{
			return false;
		}     
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = vector.Zero;
		Resource res = Resource.Load("{057AEFF961B81816}prefabs/Items/Package.et");
		if (res)
		{
			Package = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
			if (inv.TryInsertItem(Package) == false)
			{
				delete Package;
				return false;
			}
		}
		SP_PackageComponent PComp = SP_PackageComponent.Cast(Package.FindComponent(SP_PackageComponent));
		PComp.SetInfo(OName, DName, DLoc);
		
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName,OLoc, DLoc);
		TaskDesc = string.Format("Deliver package received from %1, to %2. %2 is located on %3. Reward is %4 %5", OName, DName, DLoc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskDiag = string.Format("I am looking for someone to deliver a package for me to %1 on %2. Come find me on %3. Reward is %4 %5", DName, DLoc, OLoc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskTitle = string.Format("Deliver: deliver package to %1", DName);
	};
	//------------------------------------------------------------------------------------------------------------//
	//Ready to deliver means package is in assignee's inventory, we are talking to the target and that we are assigned to task
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar != TaskTarget)
		{
			return false;
		}
		if (!CharacterAssigned(Assignee))
		{
			return false;
		}
		if (e_State == ETaskState.COMPLETED)
		{
			return false;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterRankName(TalkingChar) + " " + Diag.GetCharacterName(TalkingChar));
		array <IEntity> FoundPackages = new array <IEntity>();
		inv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			for (int i, count = FoundPackages.Count(); i < count; i++)
			{
				if (FoundPackages[i] == Package)
				{
					return true;
				}
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//overriding AssignReward to apply the average attribute from SP_RequestManagerComponent
	override bool AssignReward()
	{
		EEditableEntityLabel RewardLabel;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(!ReqMan)
			{
				return false;
			}
			SP_DeliverTask tasksample = SP_DeliverTask.Cast(ReqMan.GetTaskSample(SP_DeliverTask));
			if(!tasksample)
			{
				return false;
			}
			m_iRewardAverageAmount = tasksample.GetRewardAverage();
			if(m_iRewardAverageAmount)
			{
				m_iRewardAmount = Math.Floor(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
			}
			else
			{
				m_iRewardAmount = Math.RandomInt(5, 15)
			}
		}
		if(index == 1)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		if(!Catalog)
			{
				Print("Cant find catalog, task creation failed in Assign reward");
				return false;
			}
		SCR_EntityCatalog RewardsCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		if(!RewardsCatalog)
			{
				Print("Rewards missing from entity catalog");
				return false;
			}
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RewardsCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		if(!reward)
			{
				return false;
			}
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Complete tasks means package is on target's inventory and reward is givven to assigne
	override bool CompleteTask(IEntity Assignee)
	{
		
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Targetinv = InventoryStorageManagerComponent.Cast(TaskTarget.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundPackages = new array <IEntity>();
		Assigneeinv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundPackages[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				inv.TryRemoveItemFromStorage(FoundPackages[0],parentSlot.GetStorage());
				Targetinv.TryInsertItem(FoundPackages[0]);
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
	//Info needed for delivery mission is Names of O/T and location names of O/T
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!TaskOwner || !TaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		AIControlComponent comp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
		DName = CharRank.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget);
		OLoc = Director.GetCharacterLocation(TaskOwner);
		DLoc = Director.GetCharacterLocation(TaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//
	//delivery fails if targer is killed
	override void UpdateState()
	{
		SCR_CharacterDamageManagerComponent DmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (DmgComp.IsDestroyed())
		{
			if(m_TaskMarker)
			{
				m_TaskMarker.Fail(true);
			}
			e_State = ETaskState.FAILED;
			return;
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_DeliverTask;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetPackage(){return Package;};
	//------------------------------------------------------------------------------------------------------------//
	//delete task entity. pakcage
	override void DeleteLeftovers()
	{
		if(Package)
		{
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(Package.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			if(parentSlot)
			{
				SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
				if(inv)
				{
					inv.TryRemoveItemFromStorage(Package,parentSlot.GetStorage());
					delete Package;
				}
			}
		}
		if(Package)
		{
			delete Package;
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
	};
	//------------------------------------------------------------------------------------------------------------//
};
//------------------------------------------------------------------------------------------------------------//
class SP_PackagePredicate : InventorySearchPredicate
{
	string m_TargetName;
	//------------------------------------------------------------------------------------------------------------//
	void SP_PackagePredicate(string Name){m_TargetName = Name;};
	//------------------------------------------------------------------------------------------------------------//
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SP_PackageComponent PackageComp = SP_PackageComponent.Cast(item.FindComponent(SP_PackageComponent));
		
		if (PackageComp)
		{
			string oname;
			string tname;
			string loc;
			PackageComp.GetInfo(oname, tname, loc);
			if (m_TargetName == tname)
			{
				return true;
			}
		}
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------//