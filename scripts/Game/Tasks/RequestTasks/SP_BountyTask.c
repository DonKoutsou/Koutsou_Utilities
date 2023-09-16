[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_BountyTask: SP_Task
{
	DogTagEntity Tag;
	DogTagEntity GetTag(){return Tag;};
	//------------------------------------------------------------------------------------------------------------//
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
			SCR_FactionManager FactionMan = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			array <Faction> enemies = new array <Faction>();
			FactionMan.GetFactionsList(enemies);
			if (enemies.Contains(Fact))
				enemies.RemoveItem(Fact);
			if (enemies.IsEmpty())
				return false;
			
			if (!CharacterHolder.GetFarUnit(ChimeraCharacter.Cast(GetOwner()), 300, Char))
				return false;
		}
		if (SP_RequestManagerComponent.CharIsTargetOf(Char, GetClassName()))
			return false;
		if (SP_DialogueComponent.GetCharacterRep(Char) > 40)
			return false;
		if (Char)
			Target = Char;
		
		if (Target == GetOwner())
			return false;
		InventoryStorageManagerComponent Targerinv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(Target) + " " + Diag.GetCharacterName(Target));
		array <IEntity> FoundTags = new array <IEntity>();
		Targerinv.FindItems(FoundTags, TagPred);
		if (FoundTags.IsEmpty())
			return false;
		
		Tag = DogTagEntity.Cast(FoundTags[0]);
		
		if(Target)
			return true;
		
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);
		m_sTaskDesc = string.Format("%1 has put a bounty on %2's head. %1 is on %3, go meet him to give you more details if you are interested", OName, DName, OLoc);
		if (m_bHasReward)
		{
			string s_RewardName = FilePath.StripPath(m_Reward);
			s_RewardName = s_RewardName.Substring(0, s_RewardName.Length() - 3);
			s_RewardName.ToLower();
			m_sTaskDiag = string.Format("I've put a bounty on %1's head, last i heard he was located on %2, get me his dogtags and i'll make it worth your while. Reward is %3 %4", DName, DLoc, m_iRewardAmount, s_RewardName);
		}
			
		else
			m_sTaskDiag = string.Format("I've put a bounty on %1's head, last i heard he was located on %2, get me his dogtags.", DName, DLoc);
		m_sTaskTitle = string.Format("Bounty: retrieve %1's dogtags", DName);
		m_sTaskCompletiontext = "Thanks the completing the task %1, he got what he deserved, dont have any regrets on that.";
		m_sAcceptTest = string.Format("Give me %1's bounty.", DName);
		m_sacttext = string.Format("The bounty on %1 is complete.", DName);
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		//if(!CharacterAssigned(Assignee))
		//{
		//	return false;
		//}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (dmgman.IsDestroyed())
			return true;
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		inv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			return true;
		}
		return false;			
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool CompleteTask(IEntity Assignee)
	{
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Ownerinv = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		Assigneeinv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundTags[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				Assigneeinv.TryRemoveItemFromStorage(FoundTags[0],parentSlot.GetStorage());
				Ownerinv.TryInsertItem(FoundTags[0]);
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
				}
				e_State = ETaskState.COMPLETED;
				m_eCopletionist = Assignee;
				if (SCR_EntityHelper.GetPlayer() == Assignee)
					SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
				SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
				dmgmn.GetOnDamageStateChanged().Remove(FailTask);
				GetOnTaskFinished(this);
				if (GetGame().GetPlayerController().GetControlledEntity() != Assignee)
				{
					AIControlComponent comp = AIControlComponent.Cast(Assignee.FindComponent(AIControlComponent));
					AIAgent agent = comp.GetAIAgent();
					SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
					SCR_AIExecuteBountyTaskBehavior act = SCR_AIExecuteBountyTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteBountyTaskBehavior));
					if (act)
						act.SetActiveFollowing(false);
					UnAssignCharacter();
				}
				return true;
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!m_eTaskOwner || !m_eTaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = CharRank.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//

	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_BountyTask;};
	//------------------------------------------------------------------------------------------------------------//
	override bool CanBeAssigned(IEntity TalkingChar, IEntity Assignee)
	{
		return true;
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
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return false;
		SCR_AIExecuteBountyTaskBehavior action = new SCR_AIExecuteBountyTaskBehavior(utility, null, this);
		utility.AddAction(action);
		//if player throw popup
		AddAssigneeInvokers();
		return true;
	}
	override void SpawnTaskMarker(IEntity Assignee)
	{
		Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		m_eTaskTarget.GetWorldTransform(PrefabspawnParams.Transform);
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		m_TaskMarker.SetTitle(m_sTaskTitle);
		m_TaskMarker.SetDescription(m_sTaskDesc);
		m_TaskMarker.SetTarget(m_eTaskTarget);
		m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
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
		SCR_AIExecuteBountyTaskBehavior action = new SCR_AIExecuteBountyTaskBehavior(utility, null, this);
		utility.AddAction(action);
		return true;
	}
	override void UnAssignOwner()
	{
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		if (!super.AssignOwner())
			return;
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		SCR_AIExecuteBountyTaskBehavior action = SCR_AIExecuteBountyTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteBountyTaskBehavior));
		if (action)
			action.SetActiveFollowing(false);
		super.UnAssignOwner();
	}
	override void GetOnTargetDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		//array<ref SP_Task> tasks = {};	
		//SP_RequestManagerComponent.GetCharTargetTasks(m_eTaskTarget, tasks);
		//foreach (SP_Task task : tasks)
		//{
		//	if (task != this && task.GetClassName() == GetClassName())
		//	{
		//		task.FailTask();
		//	}
		//}
		RemoveTargetInvokers();
		//possible to fail task, if so override dis
	}
	override void GetOnOwnerDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveOwnerInvokers();
		FailTask();
		//possible to fail task, if so override dis
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
			SCR_AIExecuteBountyTaskBehavior act = SCR_AIExecuteBountyTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteBountyTaskBehavior));
			if (act)
				act.SetActiveFollowing(false);
		}
		super.UnAssignCharacter();
	}
};
//------------------------------------------------------------------------------------------------------------//
class SP_NamedTagPredicate : InventorySearchPredicate
{
	string m_OwnerName;
	//------------------------------------------------------------------------------------------------------------//
	void SP_NamedTagPredicate(string Name){m_OwnerName = Name;};
	//------------------------------------------------------------------------------------------------------------//
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		DogTagEntity tag = DogTagEntity.Cast(item);
		if (tag)
		{
			string TagOwnerName;
			tag.GetCname(TagOwnerName);
			if (TagOwnerName == m_OwnerName)
			{
				return true;
			}

		}
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------//
class SCR_AIGetBountyTaskParams : AITaskScripted
{
	static const string TASK_PORT = "Task";
	static const string TASK_OWNER_PORT		= "TaskOwner";
	static const string TASK_TARGET_PORT				= "TaskTarget";
	static const string DOGTAG_PORT = "DogTag";
		
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		TASK_PORT
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		TASK_OWNER_PORT,
		TASK_TARGET_PORT,
		DOGTAG_PORT,
	};
	override TStringArray GetVariablesOut()
    {
        return s_aVarsOut;
    }

	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SP_Task Task;
		IEntity Owner;
		IEntity Target;
		IEntity Assignee;
		GetVariableIn(TASK_PORT, Task);
		if(!Task)
		{
			NodeError(this, owner, "Invalid Task Provided!");
			return ENodeResult.FAIL;
		}
		Owner = Task.GetOwner();
		Target = Task.GetTarget();
		Assignee = Task.GetAssignee();
		SP_BountyTask Btask = SP_BountyTask.Cast(Task);
		SetVariableOut(DOGTAG_PORT, Btask.GetTag());
		SetVariableOut(TASK_OWNER_PORT, Task.GetOwner());
		SetVariableOut(TASK_TARGET_PORT, Task.GetTarget());
		return ENodeResult.SUCCESS;
	}	
};
class SCR_AIGetRetrieveTaskParams : AITaskScripted
{
	static const string TASK_PORT = "Task";
	static const string TASK_OWNER_PORT		= "TaskOwner";
	static const string AMMOUNT_PORT = "Ammount";
	static const string ITEM_PORT = "Item";
		
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		TASK_PORT
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		TASK_OWNER_PORT,
		AMMOUNT_PORT,
		ITEM_PORT,
	};
	override TStringArray GetVariablesOut()
    {
        return s_aVarsOut;
    }

	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SP_Task Task;
		IEntity Owner;
		IEntity Target;
		IEntity Assignee;
		GetVariableIn(TASK_PORT, Task);
		if(!Task)
		{
			NodeError(this, owner, "Invalid Task Provided!");
			return ENodeResult.FAIL;
		}
		Owner = Task.GetOwner();
		Target = Task.GetTarget();
		Assignee = Task.GetAssignee();
		SP_RetrieveTask Rtask = SP_RetrieveTask.Cast(Task);
		SetVariableOut(AMMOUNT_PORT, Rtask.GetRequestAmmount());
		SetVariableOut(ITEM_PORT, Rtask.GetRequestDescriptor());
		SetVariableOut(TASK_OWNER_PORT, Task.GetOwner());
		return ENodeResult.SUCCESS;
	}	
};
class DecoratorScripted_FindItemInInventory : DecoratorScripted
{
	static const string CHAR_PORT = "Character";
	static const string ITEM_PORT = "Item";
	protected override bool TestFunction(AIAgent owner)
	{
		IEntity Char;
		GetVariableIn(CHAR_PORT, Char);
		if (!Char)
			Char = owner.GetControlledEntity();
		IEntity Item;
		GetVariableIn(ITEM_PORT, Item);
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Char.FindComponent(InventoryStorageManagerComponent));
		if (!inv.Contains(Item))
			return true;
		return false;
	}
	
	protected override bool VisibleInPalette()
	{
		return true;
	}	
	
	protected override string GetOnHoverDescription()
	{
		return "DecoratorScripted_IsEqual: Compares whether 1st variable is bigger than 2nd. Supports int-int, float-float";
	}
	
	protected static ref TStringArray s_aVarsIn = {
		CHAR_PORT, ITEM_PORT
	};
	protected override TStringArray GetVariablesIn()
	{
		return s_aVarsIn;
	}
};
class LootItemFromCharacter : AITaskScripted
{
	static const string CHAR_PORT = "Character";
	static const string ITEM_PORT = "Item";
		
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		CHAR_PORT,
		ITEM_PORT,
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		IEntity Item;
		IEntity Char;
		GetVariableIn(ITEM_PORT, Item);
		GetVariableIn(CHAR_PORT, Char);
		if(!Item || !Char)
		{
			NodeError(this, owner, "Invalid Task Provided!");
			return ENodeResult.FAIL;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Char.FindComponent(InventoryStorageManagerComponent));
		if (inv.Contains(Item))
		{
			InventoryStorageManagerComponent Myinv = InventoryStorageManagerComponent.Cast(owner.GetControlledEntity().FindComponent(InventoryStorageManagerComponent));
			if (inv.TryMoveItemToStorage(Item, Myinv.FindStorageForItem(Item)))
				return ENodeResult.SUCCESS;
		}
		InventoryStorageManagerComponent inv2 = InventoryStorageManagerComponent.Cast(owner.GetControlledEntity().FindComponent(InventoryStorageManagerComponent));
		if (inv2.Contains(Item))
		{
			return ENodeResult.SUCCESS;
		}
		return ENodeResult.FAIL;
	}	
};
class DecoratorTestDamageIsDead: DecoratorTestScripted
{
	
	//------------------------------------------------------------------------------------------------
	protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			ScriptedDamageManagerComponent DMGMan = ScriptedDamageManagerComponent.Cast(controlled.FindComponent(ScriptedDamageManagerComponent));
			if (DMGMan.IsDestroyed())
			{
				return true;
			}
		}
		return false;
	}
}
class DecoratorTestDamageIsUncon: DecoratorTestScripted
{
	//------------------------------------------------------------------------------------------------
	protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			SCR_CharacterDamageManagerComponent DMGMan = SCR_CharacterDamageManagerComponent.Cast(controlled.FindComponent(SCR_CharacterDamageManagerComponent));
			if (DMGMan.GetIsUnconscious())
			{
				return true;
			}
		}
		return false;
	}
}
class SCR_AIPerformCompleteMurderAction : AITaskScripted
{
	protected static string TARGETENTITY_IN_PORT = "TargetEntity";
	
	protected static string TASK_IN_PORT = "Task";
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = 
	{
		TARGETENTITY_IN_PORT,
		TASK_IN_PORT
	};
	
	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		if(owner)
		{
			IEntity targetEntity;
			GetVariableIn(TARGETENTITY_IN_PORT, targetEntity);
			ref SP_Task task;
			GetVariableIn(TASK_IN_PORT, task);
			if (!targetEntity)
				return ENodeResult.FAIL;
			string userActionString = "SP_DialogueAction";
			
			IEntity controlledEntity = owner.GetControlledEntity();
			if (!controlledEntity)
				return ENodeResult.FAIL;
			
			//SCR_MeleeComponent MeleeComp =  SCR_MeleeComponent.Cast(controlledEntity.FindComponent(SCR_MeleeComponent));
			ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(targetEntity.FindComponent(ScriptedDamageManagerComponent));
			dmg.Kill(controlledEntity);
			//MeleeComp.PerformAttack();
			//MeleeComp.SetMeleeAttackStarted(true);
			return ENodeResult.SUCCESS;
		}
		return ENodeResult.FAIL;			
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
};