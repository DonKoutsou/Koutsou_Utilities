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
	//Stato of task using ETaskState enum
	protected ETaskState e_State = ETaskState.EMPTY;
	//-------------------------------------------------//
	//Characters assigned to this task
	ref array <IEntity> m_aTaskAssigned = new ref array <IEntity>();
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
	//Owner of task.
	IEntity GetOwner(){return m_eTaskOwner;};
	Faction GetOwnerFaction(){return m_OwnerFaction;};
	//------------------------------------------------------------------------------------------------------------//
	//Task target
	IEntity GetTarget(){return m_eTaskTarget;};
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
	string GetTaskDiag(){return m_sTaskDiag;}
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
	event void GetOnTaskFinished(SP_Task Task){if (m_eTaskOwner) RemoveOwnerInvokers(); if (m_eTaskTarget) RemoveTargetInvokers(); OnTaskFinished().Invoke(Task);};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to delete excess stuff when a task is failed or couldnt initialise
	void DeleteLeftovers(){};
	//------------------------------------------------------------------------------------------------------------//
	bool MarkedForRemoval(){return m_bMarkedForRemoval;};
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
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		//Check if char can get more tasks
		SP_RequestManagerComponent.GetCharTasks(m_eTaskOwner, tasks);
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
	//Function used durring init to assign the rewards of the task
	bool AssignReward()
	{
		if (!e_RewardLabel)
		{
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(!ReqMan)
			{
				return false;
			}
			SP_Task tasksample = ReqMan.GetTaskSample(GetClassName());
			if(!tasksample)
			{
				return false;
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
				m_iRewardAmount = Math.RandomInt(5, 15)
			}
		}
		else
			m_iRewardAmount = m_iRewardAverageAmount;
		
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(e_RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		m_Reward = entry.GetPrefab();
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Called when task if completed to give rewards to completionist
	bool GiveReward(IEntity Target)
	{
		if (m_Reward)
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = m_eTaskOwner.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(m_Reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, Target.GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				TargetInv.TryInsertItem(Rewardlist[i]);
				Movedamount += 1;
			}
			
			string curr = FilePath.StripPath(m_Reward);
			curr.ToLower();
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
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
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			GetOnTaskFinished(this);
			m_bMarkedForRemoval = 1;
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void FailTask(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("%1 has died, task failed", SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
		}
		e_State = ETaskState.FAILED;
		m_bMarkedForRemoval = 1;
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
		GetOnTaskFinished(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	//invoker stuff
	void AddOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(FailTask);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Insert(CheckUpdatedAffiliations);
		}
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(CreateDescritions);
	}
	void AddTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(FailTask);
	}
	void RemoveTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(FailTask);
	}
	void RemoveOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Remove(CreateDescritions);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Remove(CheckUpdatedAffiliations);
		}
	}
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
	//Assign character to this task
	void AssignCharacter(IEntity Character)
	{
		if (m_aTaskAssigned.Contains(Character))
			return;
		m_aTaskAssigned.Insert(Character);
		if(m_aTaskAssigned.Count() > 0 && e_State == ETaskState.UNASSIGNED)
		{
			e_State = ETaskState.ASSIGNED;
		}
		SpawnTaskMarker(Character);
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
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned[0]);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character is assigned
	bool CharacterAssigned(IEntity Character)
	{
		if(m_aTaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
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
		//-------------------------------------------------//
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
		source.Get("m_bEnabled", enable);
		if (enable)
			enabled = "ACTIVE";
		else
			enabled = "INACTIVE";
		title = source.GetClassName() + " | " + enabled;
		return true;
	}
}