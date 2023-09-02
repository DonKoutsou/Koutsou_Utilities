//------------------------------------------------------------------------------------------------------------//
enum ETaskState
{
	UNASSIGNED,
	ASSIGNED,
	COMPLETED,
	FAILED,
	EMPTY
}
//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_Task
{
	//-------------------------------------------------//
	[Attribute(defvalue : "1", desc : "Dissabled wont be randomly spawned but still exist as samples")]
	bool m_bEnabled;
	//-------------------------------------------------//
	[Attribute(defvalue : "1", desc : "Can be assigned to AI")]
	bool m_bAssignable;
	//-------------------------------------------------//
	[Attribute(defvalue : "5", desc : "Amount of rep + when completing this task")]
	int m_iRepReward;
	//-------------------------------------------------//
	[Attribute(desc : "Override with entity name in the world wich will become task owner instead of looking for an owner")]
	string m_sTaskOwnerOverride;
	//-------------------------------------------------//
	[Attribute(desc : "Override with entity name in the world wich will become task target instead of looking for an target")]
	string m_sTaskTargetOverride;
	//-------------------------------------------------//
	[Attribute("0", UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(EEditableEntityLabel))]
	EEditableEntityLabel e_RewardLabel;
	//-------------------------------------------------//
	[Attribute(defvalue: "10", desc: "Reward amount if reward end up being currency, keep at 1 for Distance based tasks (Navigate, Deliver), reward is calculated based on distance and multiplied to this value, so increase to increase multiplier")]
	int m_iRewardAverageAmount;
	//-------------------------------------------------//
	[Attribute(defvalue: "12", desc: "ammount of time the task will need to be completed in before penalties start appling. In Hours. If set to -1, no limit will be set")]
	float m_fTimeLimit;
	[Attribute(defvalue : "0", desc : "Should Fail On Limit reaching 0")]
	bool m_bFailOnTimeLimit;
	
	//-------------------------------------------------//
	//Character wich created the task
	IEntity m_eTaskOwner;
	//-------------------------------------------------//
	//Target of the task (kill task, deliver task etc... not necesarry on retrieve task)
	IEntity m_eTaskTarget;
	//-------------------------------------------------//
	// task description, should be composed in 3rd person. "Private .. is looking for .... in....."
	string m_sTaskDesc;
	//-------------------------------------------------//
	// task dialogue character will say when he is describing the task so the player can pick it.
	string m_sTaskDiag;
	//-------------------------------------------------//
	// used for UI popups
	string m_sTaskTitle;
	//-------------------------------------------------//
	//Dialogue line for completing the task
	string m_sTaskCompletiontext;
	//-------------------------------------------------//
	//action text for dialogue menu
	string m_sacttext;
	//-------------------------------------------------//
	//action text to show up for player to accept task
	string m_sAcceptTest;
	//-------------------------------------------------//
	//Faction of owner of task
	SCR_Faction m_OwnerFaction;
	//-------------------------------------------------//
	//Reward that is going to be handed to completionist *m_iRewardAmount
	ResourceName m_Reward;
	//Used for ClearTasks function in SP_DialogueComponent used to clean completed/failed tasks from main task array
	private bool m_bMarkedForRemoval;
	//-------------------------------------------------//
	//Amount of the m_Reward resource that is going to be given to completionist. Calculated average using m_iRewardAverageAmount taken from task sample in SP_RequestManagerComponent
	int m_iRewardAmount;
	//-------------------------------------------------//
	ref TaskDayTimeInfo m_TaskTimeInfo;
	//-------------------------------------------------//
	// used for when an ai has started heading towards taking a task to avoid multiple AI going for same task
	IEntity reserved;
	//-------------------------------------------------//
	//Stato of task using ETaskState enum
	protected ETaskState e_State = ETaskState.EMPTY;
	//-------------------------------------------------//
	//Characters assigned to this task
	IEntity m_aTaskAssigned;
	bool m_bOwnerAssigned;
	bool m_bPartOfChain;
	//-------------------------------------------------//
	//Character that completed this task. Filled after task is complete
	IEntity m_eCopletionist;
	//-------------------------------------------------//
	//task marker
	SP_BaseTask m_TaskMarker;
	//-------------------------------------------------//
	//Invoker for task finished
	private ref ScriptInvoker s_OnTaskFinished = new ref ScriptInvoker();
	//------------------------------------------------------------------------------------------------------------//
	bool TimeLimitHasPassed()
	{
		TaskDayTimeInfo CurrentDate = TaskDayTimeInfo.FromTimeOfTheDay();
		return m_TaskTimeInfo.HasPassed(CurrentDate);
	}
	void SetPartOfChain()
	{
		m_bPartOfChain = true;
	}
	void SetReserved(IEntity res){UnAssignOwner(); reserved = res;};
	bool IsReserved()
	{
		if (reserved)
			return true;
		return false;
	}
	bool IsOwnerAssigned()
	{
		return m_bOwnerAssigned;
	}
	void ClearReserves()
	{
		if (IsReserved())
		{
			AIControlComponent comp = AIControlComponent.Cast(reserved.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			if (agent)
			{
				SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
				SCR_AITaskPickupBehavior act = SCR_AITaskPickupBehavior.Cast(utility.FindActionOfType(SCR_AITaskPickupBehavior));
				if (act)
					act.SetActiveFollowing(false);
			}
		}
		SetReserved(null);
	};
	//Owner of task.
	IEntity GetOwner(){return m_eTaskOwner;};
	Faction GetOwnerFaction(){return m_OwnerFaction;};
	//------------------------------------------------------------------------------------------------------------//
	//Task target
	IEntity GetTarget(){return m_eTaskTarget;};
	IEntity GetAssignee(){return m_aTaskAssigned;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetCompletionist(){return m_eCopletionist;};
	//------------------------------------------------------------------------------------------------------------//
	//adds the completioninst's name to the m_sTaskCompletiontext
	string GetCompletionText(IEntity Completionist){return string.Format(m_sTaskCompletiontext, SP_DialogueComponent.GetCharacterRankName(Completionist) + " " + SP_DialogueComponent.GetCharacterSurname(Completionist));};
	//------------------------------------------------------------------------------------------------------------//
	//Returns state of task
	ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	//Returns task descritption
	string GetTaskDescription(){return m_sTaskDesc;}
	//------------------------------------------------------------------------------------------------------------//
	//Returns dialogue of task owner when giving task
	string GetTaskDiag(){return m_sTaskDiag + string.Format(" I need it done within the next %1 hours.", GetTimeLimit().ToString().ToInt());}
	//------------------------------------------------------------------------------------------------------------//
	//Return task title
	string GetTaskTitle(){return m_sTaskTitle;}
	//------------------------------------------------------------------------------------------------------------//
	//Get test for action in dialogue menu
	string GetActionText(){return m_sacttext;}
	//------------------------------------------------------------------------------------------------------------//
	//Text charactre will say once task is complete
	string GetAcceptText(){return m_sAcceptTest;}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for task finish invoker
	ScriptInvoker OnTaskFinished(){return s_OnTaskFinished;}
	//------------------------------------------------------------------------------------------------------------//
	event void GetOnTaskFinished(SP_Task Task){if (m_eTaskOwner) RemoveOwnerInvokers(); if (m_eTaskTarget) RemoveTargetInvokers(); if (m_aTaskAssigned) RemoveAssigneeInvokers(); OnTaskFinished().Invoke(Task);};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to delete excess stuff when a task is failed or couldnt initialise
	void DeleteLeftovers(){};
	//------------------------------------------------------------------------------------------------------------//
	bool MarkedForRemoval(){return m_bMarkedForRemoval;};
	void InheritFromSample()
	{
		SP_Task tasksample = SP_RequestManagerComponent.GetTaskSample(GetClassName());
		if(!tasksample)
		{
			return;
		}
		e_RewardLabel = tasksample.GetRewardLabel();
		m_iRepReward = tasksample.GetRepReward();
		m_iRewardAverageAmount = tasksample.GetRewardAverage();
		if(m_iRewardAverageAmount)
		{
			m_iRewardAmount = Math.Ceil(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
		}
		else
		{
			m_iRewardAmount = Math.RandomInt(m_iRewardAverageAmount/2, m_iRewardAverageAmount * 1.5)
		}
		m_fTimeLimit = tasksample.m_fTimeLimit;
		m_bFailOnTimeLimit = tasksample.m_bFailOnTimeLimit;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to set up all the texts of the task on Init
	void CreateDescritions(){};
	//-------------------------------------//
	//Getter for reputation reward of task
	int GetRepReward(){return m_iRepReward;};
	//-------------------------------------//
	//------------------------------------------------------------------------------------------------------------//
	//Function to set up entities used for the task. E.g. to spawn package on a delivery mission
	bool SetupTaskEntity(){return true;};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to check if task is ready to deliver. Simmilar to CanBePerformed
	bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee){return true;};
	//------------------------------------------------------------------------------------------------------------//
	//Function to test if someone can take that task
	bool CanBeAssigned(IEntity TalkingChar, IEntity Assignee)
	{
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Function to hold logic of what owner should be for each task
	bool FindOwner(out IEntity Owner)
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
	//Function to hold logic of what tarket should be for each task
	bool FindTarget(out IEntity Target){return true;};
	//------------------------------------------------------------------------------------------------------------//
	//Returns class name of task
	typename GetClassName(){return SP_Task;}
	//------------------------------------------------------------------------------------------------------------//
	//Function used to do various checks on owner character found durring initialisation
	bool CheckOwner()
	{
		if (!m_eTaskOwner)
			return false;
		if (m_eTaskOwner == SCR_EntityHelper.GetPlayer())
			return false;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		array<ref SP_Task> assignedtasks = {};
		SP_RequestManagerComponent.GetassignedTasks(m_eTaskOwner, assignedtasks);
		if (!assignedtasks.IsEmpty())
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		//Check if char can get more tasks
		SP_RequestManagerComponent.GetCharOwnedTasks(m_eTaskOwner, tasks);
		if(tasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksPerCharacter())
		{
			return false;
		}
		//Check if char can get more tasks of same type
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharTasksOfSameType(m_eTaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksOfSameTypePerCharacter())
		{
			return false;
		}
		Faction senderFaction = SP_DialogueComponent.GetCharacterFaction(m_eTaskOwner);
		if (!senderFaction)
			return false;
		m_OwnerFaction = SCR_Faction.Cast(senderFaction);
		if (m_OwnerFaction.GetFactionKey() == "RENEGADE")
		{
			return false;
		};
		if (e_RewardLabel == EEditableEntityLabel.ITEMTYPE_CURRENCY)
		{
			if (SP_RequestManagerComponent.GetCharCurrency(m_eTaskOwner) < m_iRewardAmount)
				return false;
		}
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to do various checks on target character found durring initialisation
	bool CheckTarget()
	{
		if (!m_eTaskTarget)
			return false;
		if (m_eTaskTarget == SCR_EntityHelper.GetPlayer())
			return false;
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharTargetTasks(m_eTaskTarget, tasks);
		if(tasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksPerCharacter())
		{
			return false;
		}
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharTasksOfSameType(m_eTaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksOfSameTypePerCharacter() + 1)
		{
			return false;
		}
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character provided in the owner of this task
	bool CharacterIsOwner(IEntity Character)
	{
		if (Character == m_eTaskOwner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character provided is target of this task
	bool CharacterIsTarget(IEntity Character)
	{
		if (Character == m_eTaskTarget)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character is assigned
	bool CharacterAssigned(IEntity Character)
	{
		if(m_aTaskAssigned == Character)
		{
			return true;
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------------------//
	//Manipulation of task state and results
	bool CompleteTask(IEntity Assignee)
	{
		if (GiveReward(Assignee))
		{
			if (m_TaskMarker)
			{
				m_TaskMarker.Finish(true);
			}
			e_State = ETaskState.COMPLETED;
			m_eCopletionist = Assignee;
			if (SCR_EntityHelper.GetPlayer() == Assignee)
				SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			GetOnTaskFinished(this);
			m_bMarkedForRemoval = 1;
			if (m_aTaskAssigned)
				UnAssignCharacter();
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void FailTask()
	{
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("%1 has died, task failed", SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
		}
		if (GetState() == ETaskState.ASSIGNED)
			UnAssignCharacter();
		e_State = ETaskState.FAILED;
		m_bMarkedForRemoval = 1;
		ClearReserves();
		GetOnTaskFinished(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	//Fail task duplicate used for stuff other than character dying
	void CancelTask()
	{
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("Faction relations have shifted and %1 has withdrawn his task.", SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
		}
		e_State = ETaskState.FAILED;
		m_bMarkedForRemoval = 1;
		if (m_aTaskAssigned)
			UnAssignCharacter();
		ClearReserves();
		GetOnTaskFinished(this);
	}
	///////////////////REWARD//////////////
	//------------------------------------------------------------------------------------------------------------//
	//Function used durring init to assign the rewards of the task
	bool AssignReward()
	{
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(e_RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		m_Reward = entry.GetPrefab();
		if (m_bPartOfChain && e_RewardLabel == EEditableEntityLabel.ITEMTYPE_CURRENCY)
		{
			SCR_ChimeraCharacter Character = SCR_ChimeraCharacter.Cast(m_eTaskOwner);
			WalletEntity wallet = Character.GetWallet();
			wallet.SpawnCurrency(m_iRewardAmount);
		}
		return true;
	};
	void SetTimeLimit()
	{
		if (m_fTimeLimit == -1)
			return;
		float TimeLimit = Math.RandomFloat(m_fTimeLimit/2, m_fTimeLimit * 1.5);
		m_TaskTimeInfo = TaskDayTimeInfo.FromPointInFuture(TimeLimit, ETaskTimeLimmit.HOURS);
	}
	float GetTimeLimit()
	{
		if (!m_TaskTimeInfo)
			return -1;
		TaskDayTimeInfo info = TaskDayTimeInfo.FromTimeOfTheDay();
		float limit = m_TaskTimeInfo.CalculateTimeDifferance(info);
		return limit;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Called when task if completed to give rewards to completionist
	bool GiveReward(IEntity Target)
	{
		if (Target == m_eTaskOwner)
			return true;
		if (e_RewardLabel == EEditableEntityLabel.ITEMTYPE_CURRENCY)
		{
			SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(Target);
			SCR_ChimeraCharacter OChar = SCR_ChimeraCharacter.Cast(m_eTaskOwner);
			WalletEntity wallet = Char.GetWallet();
			WalletEntity Owallet = OChar.GetWallet();
			
			
			/*int Movedamount;
			
			Resource cur = Resource.Load("{891BA05A96D3A0BE}prefabs/Currency/Drachma.et");
			PrefabResource_Predicate pred = new PrefabResource_Predicate(cur.GetResource().GetResourceName());
			array <IEntity> items = {};
			inv.FindItems(items, pred);
			for (int i, count = m_iRewardAmount; i < count; i++)
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(items[i].FindComponent(InventoryItemComponent));
				inv.TryRemoveItemFromStorage(items[i], pInvComp.GetParentSlot().GetStorage());
				TargetInv.TryInsertItem(items[i]);
				Movedamount += 1;
			}*/
			return wallet.TakeCurrency(Owallet, m_iRewardAmount);
		}
		else if (m_Reward)
		{
			
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = m_eTaskOwner.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(m_Reward);
			int Movedamount;
			Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, Target.GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				TargetInv.TryInsertItem(Rewardlist[i]);
				Movedamount += 1;
			}
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//invoker stuff
	//------------------------------------------------------------------//
	//Owner stuff
	void GetOnOwnerDeath()
	{
		RemoveOwnerInvokers();
		//m_eTaskOwner = null;
		//possible to fail task, if so override dis
	}
	void GetOnOwnerRankUp()
	{
		CreateDescritions();
	}
	void AddOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(GetOnOwnerDeath);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Insert(CheckUpdatedAffiliations);
		}
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(GetOnOwnerRankUp);
	}
	void RemoveOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(GetOnOwnerDeath);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Remove(GetOnOwnerRankUp);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Remove(CheckUpdatedAffiliations);
		}
	}
	//------------------------------------------------------------------//
	//Target stuff
	void GetOnTargetDeath()
	{
		RemoveTargetInvokers();
		//m_eTaskTarget = null;
		//possible to fail task, if so override dis
	}
	void GetOnTargetRankUp()
	{
		CreateDescritions();
	}
	void AddTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(GetOnTargetDeath);
	}
	void RemoveTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(GetOnTargetDeath);
	}
	
	//------------------------------------------------------------------//
	//Assignee stuff
	//------------------------------------------------------------------------------------------------------------//
	//Assign character to this task
	bool AssignCharacter(IEntity Character)
	{
		if (m_bOwnerAssigned)
			UnAssignOwner();
		//if already assigned return
		if (m_aTaskAssigned == Character)
			return true;
		ClearReserves();
		//If player
		if (GetGame().GetPlayerController().GetControlledEntity() == Character)
		{
			m_aTaskAssigned = Character;
			if(m_aTaskAssigned && e_State == ETaskState.UNASSIGNED)
			{
				e_State = ETaskState.ASSIGNED;
			}
			AddAssigneeInvokers();
			SpawnTaskMarker(Character);
			return true;
		}
		//if AI
		else
		{
			m_aTaskAssigned = Character;
			if(m_aTaskAssigned && e_State == ETaskState.UNASSIGNED)
			{
				e_State = ETaskState.ASSIGNED;
			}
			
			AddAssigneeInvokers();
			return true;
		}
		return false;
	}
	//Assign character to this task
	bool AssignOwner()
	{
		m_bOwnerAssigned = true;
		return true;
	}
	void UnAssignOwner()
	{
		m_bOwnerAssigned = false;
	}
	void UnAssignCharacter()
	{
		if (!m_aTaskAssigned)
			return;
		m_aTaskAssigned = null;
		if (m_TaskMarker)
		{
			m_TaskMarker.Cancel(true);
		}
		e_State = ETaskState.UNASSIGNED;
		ClearReserves();
	}
	void GetOnAssigneeDeath()
	{
		RemoveAssigneeInvokers();
		//m_aTaskAssigned = null;
		UnAssignCharacter();
		//Decide owner behevior
	}
	void AddAssigneeInvokers()
	{
		if (!m_aTaskAssigned)
			return;
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_aTaskAssigned.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(GetOnAssigneeDeath);
	}
	void RemoveAssigneeInvokers()
	{
		if (!m_aTaskAssigned)
			return;
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_aTaskAssigned.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(GetOnAssigneeDeath);
	}
	//------------------------------------------------------------------//
	void CheckUpdatedAffiliations(SCR_Faction factionA, SCR_Faction factionB = null)
	{
		if (!factionB || !m_eTaskTarget)
		{
			return;
		}
		FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(m_eTaskTarget.FindComponent(FactionAffiliationComponent));
		if (affcomp.GetAffiliatedFaction() == factionB)
			CancelTask();
	}
	
	//------------------------------------------------------------------------------------------------------------//
	//Spawn task marker for this task, called when assigning character
	void SpawnTaskMarker(IEntity Assignee)
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
	
	//------------------------------------------------------------------------------------------------------------//
	//Getter of m_iRewardAverageAmount attribute. Used on task samples stored in SP_RequestManagerComponent.
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
	};
	//Reward lebal deffines what kind of reward this task will provide. atm only works with EEditableEntityLabel.ITEMTYPE_ITEM and EEditableEntityLabel.ITEMTYPE_WEAPON
	EEditableEntityLabel GetRewardLabel()
	{
		return e_RewardLabel;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Deffining structure of Init
	bool Init()
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
		//-------------------------------------------------//
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		//function to fill to check ckaracter
		if(!CheckOwner())
		{
			return false;
		}
		if(!CheckTarget())
		{
			return false;
		}
		//-------------------------------------------------//
		SetTimeLimit();
		CreateDescritions();
		AddOwnerInvokers();
		AddTargetInvokers();
		e_State = ETaskState.UNASSIGNED;
		//Set the invoker so the task fails if owner dies
		
		return true;
	};
	
};
class TaskAttribute : BaseContainerCustomTitle
{
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		string enabled;
		bool enable;
		bool assign;
		string assignable;
		source.Get("m_bEnabled", enable);
		if (enable)
			enabled = "ACTIVE";
		else
			enabled = "INACTIVE";
		
		source.Get("m_bAssignable", assign);
		
		if (assign)
			assignable = "Assignable";
		else
			assignable = "UnAssignable";
		
		title = source.GetClassName() + " | " + enabled + " | " + assignable;
		return true;
	}
}
class SCR_AIGetTaskParams : AITaskScripted
{
	static const string TASK_PORT = "Task";
	static const string TASK_OWNER_PORT		= "TaskOwner";
	static const string TASK_TARGET_PORT				= "TaskTarget";
		
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
	};
	override TStringArray GetVariablesOut()
    {
        return s_aVarsOut;
    }

	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SP_Task Task;
		GetVariableIn(TASK_PORT, Task);
		if(!Task)
		{
			//NodeError(this, owner, "Invalid Task Provided!");
			return ENodeResult.FAIL;
		}
		SetVariableOut(TASK_OWNER_PORT, Task.GetOwner());
		SetVariableOut(TASK_TARGET_PORT, Task.GetTarget());
		return ENodeResult.SUCCESS;
	}	
};
class TaskDayTimeInfo
{
	ref TimeContainer m_fTimeOfDay;
	ref TaskDateInfo Date;
	
	void TaskDayTimeInfo(float Time, int Day, int Month, int Year)
	{
		Date = new TaskDateInfo(Day, Month, Year);
		m_fTimeOfDay = TimeContainer.FromTimeOfTheDay(Time);
	}
	void GetDate(out float Time, out int Day, out int Month, out int Year)
	{
		Date.GetDate(Day, Month, Year);
		m_fTimeOfDay = TimeContainer.FromTimeOfTheDay(Time);
		Time = m_fTimeOfDay.ToTimeOfTheDay();
	}
	TimeContainer GetTime()
	{
		return m_fTimeOfDay;
	};
	TaskDateInfo GetDateInfo()
	{
		return Date;
	};
	bool HasPassed(TaskDayTimeInfo DayInfo)
	{
		if (Date.HasPassed(DayInfo.GetDateInfo()))
			return true;
		TimeContainer Time = DayInfo.GetTime();
		if (Time.ToTimeOfTheDay() > m_fTimeOfDay.ToTimeOfTheDay())
			return true;
		return false;
	}
	float CalculateTimeDifferance(TaskDayTimeInfo Info)
	{
		float dif;
		float Time = Info.GetTime().ToTimeOfTheDay();
		dif += m_fTimeOfDay.ToTimeOfTheDay() - Time;
		if (Info.GetDateInfo().HasPassed(Date))
		{
			int DateDiff = Info.GetDateInfo().CalculateTimeDifferance(Date);
			dif += DateDiff;
		}
		return dif;
	}
	static TaskDayTimeInfo FromTimeOfTheDay()
	{
		TimeAndWeatherManagerEntity timenw = GetGame().GetTimeAndWeatherManager();
		float Time = timenw.GetTime().ToTimeOfTheDay();
		int Day, Month, Year;
		timenw.GetDate(Year, Month, Day);
		return new TaskDayTimeInfo(Time ,Day, Month, Year);
	}
	static TaskDayTimeInfo FromPointInFuture(int Ammount, ETaskTimeLimmit TimeLimmit)
	{
		TimeAndWeatherManagerEntity timenw = GetGame().GetTimeAndWeatherManager();
		float Time = timenw.GetTime().ToTimeOfTheDay();
		int Day, Month, Year;
		timenw.GetDate(Year, Month, Day);
		if (TimeLimmit == ETaskTimeLimmit.HOURS)
		{
			Time += Ammount;
		}
		while (Time > 24)
		{
			Time -= 24;
			Day += 1;
		}
		while (Day > TimeAndWeatherManagerEntity.GetAmountOfDaysInMonth(Month))
		{
			Day -= TimeAndWeatherManagerEntity.GetAmountOfDaysInMonth(Month);
			Month += 1;
		}
		return new TaskDayTimeInfo(Time ,Day, Month, Year);
	}
};
class TaskDateInfo
{
	int m_iDay;
	int m_iMonth;
	int m_iYear;
	void TaskDateInfo(int Day, int Month, int Year)
	{
		m_iDay = Day;
		m_iMonth = Month;
		m_iYear = Year;
	}
	void GetDate(out int Day, out int Month, out int Year)
	{
		Day = m_iDay;
		Month = m_iMonth;
		Year = m_iYear;
	}
	bool HasPassed(TaskDateInfo info)
	{
		int Day;
		int Month;
		int Year;
		info.GetDate(Day, Month, Year);
		if (Year > m_iYear)
			return true;
		if (Month > m_iMonth)
			return true;
		if (Day > m_iDay)
			return true;
		return false;
		
	}
	float CalculateTimeDifferance(TaskDateInfo DateInfo)
	{
		float dif;
		int Day;
		int Month;
		int Year;
		DateInfo.GetDate(Day, Month, Year);
		if (Day > m_iDay)
		{
			for (int i, count = Day - m_iDay; i < count; i++)
			{
				dif += 24;
			}
		}
		if (Month > m_iMonth)
		{
			for (int i, count = Month - m_iMonth; i < count; i++)
			{
				dif += 24 * TimeAndWeatherManagerEntity.GetAmountOfDaysInMonth(m_iMonth);
			}
		}
		if (Year > m_iYear)
		{
			for (int i, count = Year - m_iYear; i < count; i++)
			{
				dif += 8760;
			}
		}
		return dif;
	}
}
enum ETaskTimeLimmit
{
	HOURS,
	DAYS,
	WEEKS,
	YEARS,
}
enum ETaskType
{
	KILL,
	DELIVER,
	NAVIGATE,
	RESCUE,
	BOUNTY,
	RETRIEVE
}