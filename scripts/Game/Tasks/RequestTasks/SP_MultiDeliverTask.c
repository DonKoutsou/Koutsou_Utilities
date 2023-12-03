//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_MultiDeliverTask: SP_Task
{
	[Attribute(defvalue: "300", desc: "Min distance for task to be valid")]
	int m_iTargetOwnerMinDistance;
	[Attribute(defvalue : "")]
	ref array <ResourceName> m_aPackages;
	//----------------------------------------------------------------------------------//
	
	//----------------------------------------------------------------------------------//
	//Package that needs to be delivered
	ref array <IEntity> m_ePackage = {};


	//------------------------------------------------------------------------------------------------------------//
	//Setup delivery package
	override bool SetupTaskEntity()
	{
		SP_MultiDeliverTask tasksample = SP_MultiDeliverTask.Cast(SP_RequestManagerComponent.GetTaskSample(SP_MultiDeliverTask));
		
		
		for (int i = 0, count = tasksample.m_aPackages; i < count; i++)
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
			Resource res = Resource.Load(tasksample.m_aPackages[i]);
			if (res)
			{
				m_ePackage.Insert( GetGame().SpawnEntityPrefab( res, GetGame().GetWorld(), params ) );
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast( m_eTaskOwner.FindComponent(InventoryStorageManagerComponent ) );
				if ( ! inv.TryInsertItem(m_ePackage[i]) )
				{
					delete m_ePackage[i];
					return false;
				}
			}
			SP_PackageComponent PComp = SP_PackageComponent.Cast( m_ePackage[i].FindComponent(SP_PackageComponent) );
			PComp.SetInfo( OName, DName, DLoc );
		}
		
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	override void CreateDescritions()
	{
		array <string> DnameList;
		string DName;
		string OName;
		string DLoc;
		string OLoc;
		for (int i = 0, count = m_aPackages.Count(); i < count; i++)
		{
			string DnameTemp;
			GetInfo(OName, DnameTemp ,OLoc, DLoc);
			if (i == m_aPackages.Count() - 1)
				DName = DName + " and " + DnameTemp;
			else
				DName = DName + ", " + DnameTemp;
		}
		
		
		m_sTaskDesc = string.Format("%1 is looking for someone to do some deliveries for me. Targets are %1, their locations are on the package", DName);
		m_sTaskTitle = string.Format("Deliver %1's package to %2.", OName, DName);
		if (m_bHasReward && a_Rewards.Get(0))
		{
			string s_RewardName = FilePath.StripPath(a_Rewards.Get(0).GetPrefabData().GetPrefabName());
			s_RewardName = s_RewardName.Substring(0, s_RewardName.Length() - 3);
			s_RewardName.ToLower();
			m_sTaskDiag = string.Format("I am looking for someone to deliver a package to %1, around %2. Reward is %3 %4", DName, DLoc, a_Rewards.Count(), s_RewardName);
			m_sTaskCompletiontext = string.Format("Thanks %1, your %2 %3, you erned them.", "%1", a_Rewards.Count(), s_RewardName);
		}
		else
		{
			m_sTaskDiag = string.Format("I am looking for someone to deliver a package to %1, around %2.", DName, DLoc);
			m_sTaskCompletiontext = string.Format("Thanks %1.", "%1");
		}
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
		DS_DialogueComponent Diag = DS_DialogueComponent.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetDialogueComponent());
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
		DS_DialogueComponent Diag = DS_DialogueComponent.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget));
		array <IEntity> FoundPackages = new array <IEntity>();
		Assigneeinv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				if (!m_aTaskAssigned)
					m_aTaskAssigned = Assignee;
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundPackages[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				Assigneeinv.TryRemoveItemFromStorage(FoundPackages[0],parentSlot.GetStorage());
				DeleteLeftovers();
				//if (m_TaskMarker)
				//{
				//	m_TaskMarker.Finish(true);
				//}
				e_State = ETaskState.COMPLETED;
				m_eCopletionist = Assignee;
				if (SCR_EntityHelper.GetPlayer() == Assignee)
					SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
				SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
				dmgmn.GetOnDamageStateChanged().Remove(FailTask);
				GetOnTaskFinished(this);
				if (GetGame().GetPlayerController().GetControlledEntity() != Assignee)
				{
					AIControlComponent comp = AIControlComponent.Cast(Assignee.FindComponent(AIControlComponent));
					AIAgent agent = comp.GetAIAgent();
					SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
					SCR_AIExecuteDeliveryTaskBehavior act = SCR_AIExecuteDeliveryTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteDeliveryTaskBehavior));
					if (act)
						act.SetActiveFollowing(false);
					AIControlComponent Tcomp = AIControlComponent.Cast(m_eTaskTarget.FindComponent(AIControlComponent));
					AIAgent Tagent = Tcomp.GetAIAgent();
					SCR_AIGroup Tgroup = SCR_AIGroup.Cast(Tagent.GetParentGroup());
					if (Tgroup)
					{
						AIWaypoint wp;
						wp = Tgroup.GetCurrentWaypoint();
						agent.GetParentGroup().AddWaypoint(wp);
					}
					UnAssignCharacter();
				}
				return true;
			}
		}
		return false;
	};
	override void FailTask()
	{
		super.FailTask();
		DeleteLeftovers();
	}
	//------------------------------------------------------------------------------------------------------------//
	//Fail task duplicate used for stuff other than character dying
	override void CancelTask()
	{
		super.CancelTask();
		DeleteLeftovers();
	}
	//------------------------------------------------------------------------------------------------------------//
	//Info needed for delivery mission is Names of O/T and location names of O/T
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!m_eTaskOwner || !m_eTaskTarget)
		{
			return;
		}
		SCR_GameModeCampaign GM = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		if (!GM)
			return;
		DS_DialogueComponent Diag = GM.GetDialogueComponent();
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = Diag.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_MultiDeliverTask;};
	
	//------------------------------------------------------------------------------------------------------------//
	array <IEntity> GetPackages(){return m_ePackage;};
	//------------------------------------------------------------------------------------------------------------//
	//delete task entity. pakcage
	override void DeleteLeftovers()
	{
		for (int i = 0, count = m_ePackage.Count(); i < count; i++)
		{
			if(m_ePackage[i])
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(m_ePackage[i].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				if(parentSlot)
				{
					SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
					if(inv)
					{
						inv.TryRemoveItemFromStorage(m_ePackage[i],parentSlot.GetStorage());
						delete m_ePackage[i];
					}
				}
				else
					delete m_ePackage[i];
			}
			if(m_ePackage[i])
			{
				delete m_ePackage[i];
			}
		}
	};
	override bool CanBeAssigned(IEntity TalkingChar, IEntity Assignee)
	{
		if (m_aTaskAssigned)
			return false;
		for (int i = 0, count = m_ePackage.Count(); i < count; i++)
		{
			
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
			BaseInventoryStorageComponent storage = inv.FindStorageForItem(m_ePackage[i]);
			if(!storage)
			{
				return false;
			}
		}
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignCharacter(IEntity Character)
	{
		if (Character == m_eTaskOwner)
			return false;
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
		for (int i = 0, count = m_ePackage.Count(); i < count; i++)
		{
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(m_ePackage[i].FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			BaseInventoryStorageComponent storage = inv.FindStorageForItem(m_ePackage[i]);
			if(storage)
			{
				string storageent = storage.GetUIInfo().GetName();
				if (!invChar.TryRemoveItemFromStorage(m_ePackage[i], parentSlot.GetStorage()))
					return false;
				if (!inv.TryInsertItemInStorage(m_ePackage[i], storage))
					return false;
				if (GetGame().GetPlayerController().GetControlledEntity() == Character)
					SCR_HintManagerComponent.GetInstance().ShowCustom("The package has been added to your inventory");
			}
			else
			{
				if (SCR_EntityHelper.GetPlayer() == Character)
					SCR_HintManagerComponent.GetInstance().ShowCustom("No space in inventory, package left on the floor");
				return true;
			}
		}
		if (super.AssignCharacter(Character) && SCR_EntityHelper.GetPlayer() != Character)
		{
			AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
			if (!comp)
				return false;
			AIAgent agent = comp.GetAIAgent();
			if (!agent)
				return false;
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			if (!utility)
				return false;
			SCR_AIExecuteDeliveryTaskBehavior action = new SCR_AIExecuteDeliveryTaskBehavior(utility, null, this);
			utility.AddAction(action);
			return true;
		}
			
		return false;
	}
	override bool AssignOwner()
	{
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return false;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return false;
		if (!super.AssignOwner())
			return false;
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return false;
		SCR_AIExecuteDeliveryTaskBehavior action = new SCR_AIExecuteDeliveryTaskBehavior(utility, null, this);
		utility.AddAction(action);
		return true;
	}
	override void UnAssignOwner()
	{
		super.UnAssignOwner();
		if (!m_eTaskOwner)
			return;
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		SCR_AIExecuteDeliveryTaskBehavior action = SCR_AIExecuteDeliveryTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteDeliveryTaskBehavior));
		if (action)
			action.SetActiveFollowing(false);
		
	}
	override bool Init()
	{
		if (!m_bPartOfChain)
			InheritFromSample();
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
			
		}
		//-------------------------------------------------//
		if (!m_eTaskTarget)
		{
			if (!FindTarget(m_eTaskTarget))
			{
				return false;
			}
			
		}
		
		//-------------------------------------------------//
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		if (!CheckOwner())
		{
			return false;
		}
		if (!CheckTarget())
		{
			return false;
		}
		//-------------------------------------------------//
		//function to fill if task needs an entity, eg package for delivery
		if (!SetupTaskEntity())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		SetTimeLimit();
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
		RankCo.s_OnRankChanged.Insert(GetOnOwnerRankUp);
	}
	override void RemoveOwnerInvokers()
	{
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Remove(GetOnOwnerRankUp);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Remove(CheckUpdatedAffiliations);
		}
	}
	override bool AssignReward()
	{
		float dis = vector.Distance(m_eTaskTarget.GetOrigin(), m_eTaskOwner.GetOrigin());
		m_iRewardAmount = 1 * (dis/40);
		if (!super.AssignReward())
			return false;
		return true;
	};
	override void GetOnOwnerDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveOwnerInvokers();
		//possible to fail task, if so override dis
	}
	override void GetOnTargetDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveTargetInvokers();
		FailTask();
	}
	override void GetOnAssigneeDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		UnAssignCharacter();
		//Decide owner behevior
		//possible retrieve quest for player
	}
	override void UnAssignCharacter()
	{
		if (!m_aTaskAssigned)
			return;
		RemoveAssigneeInvokers();
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_aTaskAssigned.FindComponent(ScriptedDamageManagerComponent));
		if (!dmgman.IsDestroyed())
		{
			AIControlComponent comp = AIControlComponent.Cast(m_aTaskAssigned.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			SCR_AIExecuteDeliveryTaskBehavior act = SCR_AIExecuteDeliveryTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteDeliveryTaskBehavior));
			if (act)
				act.SetActiveFollowing(false);
		}
		super.UnAssignCharacter();
	}
	override void InheritFromSample()
	{
		super.InheritFromSample();
		SP_MultiDeliverTask TaskSample = SP_MultiDeliverTask.Cast(SP_RequestManagerComponent.GetTaskSample(GetClassName()));
		if (TaskSample)
		{
			m_iTargetOwnerMinDistance = TaskSample.m_iTargetOwnerMinDistance;
		}
	}
	//------------------------------------------------------------------------------------------------------------//
};