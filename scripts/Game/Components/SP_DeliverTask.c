//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_DeliverTask: SP_Task
{
	[Attribute(defvalue : "{057AEFF961B81816}prefabs/Items/Package.et")]
	ResourceName m_pPackage;
	//----------------------------------------------------------------------------------//
	
	//----------------------------------------------------------------------------------//
	//Package that needs to be delivered
	IEntity m_ePackage;
	//------------------------------------------------------------------------------------------------------------//
	//Delivery mission is looking for a random owner.
	override bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
		}
		else
		{
			if(!CharacterHolder.GetRandomUnit(Char))
				return false;
		}
		if (Char)
			Owner = Char;
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
		ChimeraCharacter Char;
		if (m_sTaskTargetOverride && GetGame().FindEntity(m_sTaskTargetOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskTargetOverride));
		}
		else
		{
			FactionAffiliationComponent AffiliationComp = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
			SP_FactionManager FactionMan = SP_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			array <Faction> enemies = new array <Faction>();
			FactionMan.GetFriendlyFactions(Fact, enemies);
			if (enemies.IsEmpty())
				return false;
			
			if (!CharacterHolder.GetFarUnitOfFaction(ChimeraCharacter.Cast(GetOwner()), 300, enemies.GetRandomElement(), Char))
				return false;
		}

		if (Char)
			Target = Char;
		
		if (Target == GetOwner())
			return false;
		
		if(Target)
			return true;
		
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
		SP_DeliverTask tasksample = SP_DeliverTask.Cast(SP_RequestManagerComponent.GetTaskSample(SP_DeliverTask));
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = vector.Zero;
		Resource res = Resource.Load(tasksample.m_pPackage);
		if (res)
		{
			m_ePackage = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
			if (inv.TryInsertItem(m_ePackage) == false)
			{
				delete m_ePackage;
				return false;
			}
		}
		SP_PackageComponent PComp = SP_PackageComponent.Cast(m_ePackage.FindComponent(SP_PackageComponent));
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
		string s_RewardName = FilePath.StripPath(m_Reward);
		s_RewardName = s_RewardName.Substring(0, s_RewardName.Length() - 3);
		s_RewardName.ToLower();
		m_sTaskDesc = string.Format("%1 is looking for someone to deliver a package to %2. Location: %3.", OName, DName, DLoc);
		m_sTaskDiag = string.Format("I am looking for someone to deliver a package to %1, around %2. Reward is %3 %4", DName, DLoc, m_iRewardAmount, s_RewardName);
		m_sTaskTitle = string.Format("Deliver %1's package to %2.", OName, DName);
		m_sTaskCompletiontext = string.Format("Thanks %1, your %2 %3, you erned them.", "%1", m_iRewardAmount, s_RewardName);
		m_sAcceptTest = string.Format("Give me the delivery to %1.", DName);
		m_sacttext = string.Format("I have a delivery for you from %1.", OName);
	};
	//------------------------------------------------------------------------------------------------------------//
	//Ready to deliver means package is in assignee's inventory, we are talking to the target and that we are assigned to task
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar != m_eTaskTarget)
		{
			return false;
		}
		//if (!CharacterAssigned(Assignee))
		//{
		//	return false;
		//}
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
				if (FoundPackages[i] == m_ePackage)
				{
					return true;
				}
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Complete tasks means package is on target's inventory and reward is givven to assigne
	override bool CompleteTask(IEntity Assignee)
	{
		
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Targetinv = InventoryStorageManagerComponent.Cast(m_eTaskTarget.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget));
		array <IEntity> FoundPackages = new array <IEntity>();
		Assigneeinv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundPackages[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				Assigneeinv.TryRemoveItemFromStorage(FoundPackages[0],parentSlot.GetStorage());
				Targetinv.TryInsertItem(FoundPackages[0]);
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
				}
				e_State = ETaskState.COMPLETED;
				m_eCopletionist = Assignee;
				SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
				SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
				dmgmn.GetOnDamageStateChanged().Remove(FailTask);
				GetOnTaskFinished(this);
				return true;
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Info needed for delivery mission is Names of O/T and location names of O/T
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!m_eTaskOwner || !m_eTaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = Diag.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_DeliverTask;};
	
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetPackage(){return m_ePackage;};
	//------------------------------------------------------------------------------------------------------------//
	//delete task entity. pakcage
	override void DeleteLeftovers()
	{
		if(m_ePackage)
		{
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(m_ePackage.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			if(parentSlot)
			{
				SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
				if(inv)
				{
					inv.TryRemoveItemFromStorage(m_ePackage,parentSlot.GetStorage());
					delete m_ePackage;
				}
			}
		}
		if(m_ePackage)
		{
			delete m_ePackage;
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	override void AssignCharacter(IEntity Character)
	{
		IEntity Package = GetPackage();
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
		InventoryItemComponent pInvComp = InventoryItemComponent.Cast(Package.FindComponent(InventoryItemComponent));
		InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
		invChar.TryRemoveItemFromStorage(Package, parentSlot.GetStorage());
		if(inv.TryInsertItem(Package))
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom("The package has been added to your inventory");
		}
		else
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom("No space in inventory, package left on the floor");
		}
		super.AssignCharacter(Character);
	}
	override bool Init()
	{
		//-------------------------------------------------//
		//first look for owner cause targer is usually derived from owner faction/location etc...
		if (!m_eTaskOwner)
		{
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
		//-------------------------------------------------//
		if (!m_eTaskTarget)
		{
			if (!FindTarget(m_eTaskTarget))
			{
				return false;
			}
			if(!CheckTarget())
			{
				return false;
			}
		}
		//-------------------------------------------------//
		//function to fill if task needs an entity, eg package for delivery
		if (!SetupTaskEntity())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		CreateDescritions();
		AddOwnerInvokers();
		AddTargetInvokers();
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	override void AddOwnerInvokers()
	{
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Insert(CheckUpdatedAffiliations);
		}
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(CreateDescritions);
	}
	override void RemoveOwnerInvokers()
	{
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Remove(CreateDescritions);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Remove(CheckUpdatedAffiliations);
		}
	}
	override bool AssignReward()
	{
		if (!super.AssignReward())
			return false;
		float dis = vector.Distance(m_eTaskTarget.GetOrigin(), m_eTaskOwner.GetOrigin());
		m_iRewardAmount = m_iRewardAmount * (dis/40);
		return true;
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