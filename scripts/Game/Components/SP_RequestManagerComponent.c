//////////////////////////////////////////////////////////////
//---------------------Request Manager---------------------//
class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
[BaseContainerProps(configRoot: true)]
class SP_RequestManagerComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------------------//
	[Attribute(defvalue : "3", desc: "Cooldown for task generation")]
	float m_fTaskGenTime;
	//------------------------------------------------------------------------------------------------------------//
	protected int m_iMinTaskAmount;
	//------------------------------------------------------------------------------------------------------------//
	[Attribute(defvalue: "2", desc: "Max amount of tasks of sametype a character can be requesting at the same time")]
	protected int m_fTaskOfSameTypePerCharacter;
	//------------------------------------------------------------------------------------------------------------//
	[Attribute(desc: "Type of tasks that will be created by request manager. Doesent stop from creating different type of task wich doesent exist here.")]
	ref array<ref SP_Task> m_aTasksToSpawn;
	//------------------------------------------------------------------------------------------------------------//
	[Attribute(desc: "Type of tasks that will be created by request manager. Doesent stop from creating different type of task wich doesent exist here.")]
	ref array<ref SP_ChainedTask> m_aQuestlines;
	//------------------------------------------------------------------------------------------------------------//
	bool m_bQuestInited;
	//------------------------------------------------------------------------------------------------------------//
	//Tasks samples to set up settings for all different tasks
	static ref array<ref SP_Task> m_aTaskSamples = null;
	//------------------------------------------------------------------------------------------------------------//
	//Garbage Manager
	[Attribute(defvalue: "60", desc: "Task garbage manager kinda. Completed task are added to their own list, failed tasks are deleted")]
	float m_fTaskClearTime;
	//------------------------------------------------------------------------------------------------------------//
	[Attribute(defvalue:"0")]
	bool m_bShowDebug;
	//------------------------------------------------------------------------------------------------------------//
	protected float m_fTaskRespawnTimer;
	protected float m_fTaskClearTimer;
	protected float m_fDebugTimer;
	SCR_GameModeCampaign m_GameMode;
	//------------------------------------------------------------------------------------------------------------//
	protected ref CharacterHolder m_CharacterHolder;
	//------------------------------------------------------------------------------------------------------------//
	//Array of existing tasks
	static ref array<ref SP_Task> m_aTaskMap = null;
	//Array of Completed tasks
	static ref array<ref SP_Task> m_aCompletedTaskMap = null;
	//------------------------------------------------------------------------------------------------------------//
	//Invokers
	private ref ScriptInvoker s_OnTaskComplete = new ref ScriptInvoker();
	private ref ScriptInvoker s_OnTaskFailed = new ref ScriptInvoker();
	private ref ScriptInvoker s_OnTaskCreated = new ref ScriptInvoker();
	//------------------------------------------------------------------------------------------------------------//
	//Constructor
	void SP_RequestManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent){if (!s_Instance)s_Instance = this;};
	//Destructor
	void ~SP_RequestManagerComponent(){if (m_aCompletedTaskMap)m_aCompletedTaskMap.Clear();if (m_aTaskMap)m_aTaskMap.Clear();if (m_aTaskSamples)m_aTaskSamples = null;};
	//------------------------------------------------------------------------------------------------------------//
	//instance
	static SP_RequestManagerComponent s_Instance;
	static sealed SP_RequestManagerComponent GetInstance(){return s_Instance;}
	//------------------------------------------------------------------------------------------------------------//
	ScriptInvoker OnTaskComplete()
	{
		return s_OnTaskComplete;
	}
	//------------------------------------------------------------------------------------------------------------//
	ScriptInvoker OnTaskFail()
	{
		return s_OnTaskFailed;
	}
	//------------------------------------------------------------------------------------------------------------//
	ScriptInvoker OnTaskCreated()
	{
		return s_OnTaskCreated;
	}
	//------------------------------------------------------------------------------------------------------------//
	event void OnTaskCompleted(SP_Task Task)
	{
		m_aTaskMap.RemoveItem(Task);
		m_aCompletedTaskMap.Insert(Task);
		OnTaskComplete().Invoke(Task, Task.GetCompletionist());
	};
	//------------------------------------------------------------------------------------------------------------//
	event void OnTaskFailed(SP_Task Task)
	{
		m_aTaskMap.RemoveItem(Task);
		OnTaskFail().Invoke(Task, Task.GetCompletionist());
	};
	//------------------------------------------------------------------------------------------------------------//
	void OnTaskFinished(SP_Task task)
	{
		task.OnTaskFinished().Remove(OnTaskFinished);
		ETaskState state = task.GetState();
		if (state == ETaskState.COMPLETED)
			OnTaskCompleted(task);
		if (state == ETaskState.FAILED)
			OnTaskFailed(task);
	}
	//------------------------------------------------------------------------------------------------------------//
	/*int GetTasksPerCharacter()
	{
		return m_fTaskPerCharacter;
	}*/
	//------------------------------------------------------------------------------------------------------------//
	int GetTasksOfSameTypePerCharacter()
	{
		return m_fTaskOfSameTypePerCharacter;
	}
	CharacterHolder GetCharacterHolder()
	{
		return m_CharacterHolder;
	}
	//--------------------------------------------------------------------//
	//Getter for samples
	static SP_Task GetTaskSample(typename tasktype)
	{
		if (m_aTaskSamples.IsEmpty())
			return null;
		foreach(SP_Task Task : m_aTaskSamples)
		{
			if(Task.GetClassName() == tasktype)
			{
				return Task;
			}
		}
		return null;
	}
	static array <SP_Task> GetActiveSamples()
	{
		array <SP_Task> ActiveTaskList = {};
		for (int i; i < m_aTaskSamples.Count(); i++)
		{
			if (m_aTaskSamples[i].m_bEnabled)
				ActiveTaskList.Insert(m_aTaskSamples[i]);
		}
		return ActiveTaskList;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks bool inside task
	static bool IsAssignable(typename tasktype)
	{
		foreach(SP_Task Task : m_aTaskSamples)
		{
			if(Task.GetClassName() == tasktype)
			{
				if (Task.m_bAssignable)
					return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if entity provided has tasks
	static bool CharHasTask(IEntity Char)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char))
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool CharHasOwnAssigned(IEntity Char)
	{
		array <ref SP_Task> tasks = {};
		GetCharOwnedTasks(Char, tasks);
		if (tasks.IsEmpty())
			return false;
		foreach (SP_Task task : tasks)
		{
			if (task.IsOwnerAssigned())
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool CharHasAssigned(IEntity Char)
	{
		array <ref SP_Task> tasks = {};
		GetassignedTasks(Char, tasks);
		if (tasks.IsEmpty())
			return true;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool CharFollowingAnybody(IEntity Assignee)
	{
		AIControlComponent comp = AIControlComponent.Cast(Assignee.FindComponent(AIControlComponent));
		if (!comp)
			return false;
			
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return false;
			
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return false;
		
		SCR_AIFollowBehavior followact = SCR_AIFollowBehavior.Cast(utility.FindActionOfType(SCR_AIFollowBehavior));
		if (followact)
			return true;
		
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if entity provided is target of any of the tasks
	static bool CharIsTarget(IEntity Char)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsTarget(Char))
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if entity provided is target of any of the tasks
	static bool CharIsTargetOf(IEntity Char, typename tasktype)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.GetClassName() == tasktype && task.CharacterIsTarget(Char))
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool CharIsPickingTask(IEntity Char)
	{
		if (!Char)
			return false;
		AIControlComponent comp = AIControlComponent.Cast(Char.FindComponent(AIControlComponent));
		
		AIAgent agent = comp.GetAIAgent();
		if ( !agent )
			return true;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		SCR_AITaskPickupBehavior act = SCR_AITaskPickupBehavior.Cast(utility.FindActionOfType(SCR_AITaskPickupBehavior));
		if (act)
			return true;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get assigned tasks of character
	static bool GetassignedTasks(IEntity Char, out array<ref SP_Task> tasks)
	{
		
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterAssigned(Char))
			{
				tasks.Insert(task);
			}
		}
		if (tasks.IsEmpty())
			return false;
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get assigned tasks count of character
	static int GetassignedTaskCount(IEntity Char)
	{
		array<ref SP_Task> tasks = {};
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterAssigned(Char))
			{
				tasks.Insert(task);
			}
		}
		if (tasks.IsEmpty())
			return 0;
		return tasks.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	//Create tasks of type
	bool CreateTask(typename TaskType)
	{
		if(!TaskType)
		{
			return false;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(m_GameMode.GetDialogueComponent());
		SP_Task Task = SP_Task.Cast(TaskType.Spawn());
		if(Task.Init())
		{
			IEntity Owner = Task.GetOwner();
			m_aTaskMap.Insert(Task);
			Task.OnTaskFinished().Insert(OnTaskFinished);
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	private bool CreateChainedTasks()
	{
		bool anyfailed;
		foreach (SP_ChainedTask qLine : m_aQuestlines)
		{
			InitChainedTask(qLine);
		}
		m_bQuestInited = true;
		return true;
	}
	void InitChainedTask(SP_ChainedTask qline)
	{
		if (m_aTaskMap.Contains(qline))
				return;
		if (qline.Init())
		{
			m_aTaskMap.Insert(qline);
			qline.OnTaskFinished().Insert(OnTaskFinished);
		}
		else
			GetGame().GetCallqueue().CallLater(InitChainedTask, 1000, false, qline);
			
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CreateCustomTask(typename TaskType, IEntity Owner)
	{
		if(!TaskType)
		{
			return false;
		}
		if (!GetTaskSample(TaskType).m_bEnabled)
			return false;
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(m_GameMode.GetDialogueComponent());
		SP_Task Task = SP_Task.Cast(TaskType.Spawn());
		Task.m_eTaskOwner = Owner;
		if(Task.Init())
		{
			m_aTaskMap.Insert(Task);
			Task.OnTaskFinished().Insert(OnTaskFinished);
			return true;
		}
		return false;
	}
	bool CreateCustomRetrieveTask(IEntity Owner, ERequestRewardItemDesctiptor Need, int ammount, BaseMagazineComponent Mag = null)
	{
		typename TaskType = SP_RetrieveTask;
		if (!GetTaskSample(TaskType))
			return false;
		SP_DialogueComponent Diag = SP_DialogueComponent.GetInstance();
		SP_RetrieveTask Task = SP_RetrieveTask.Cast(TaskType.Spawn());
		
		Task.m_eTaskOwner = Owner;
		Task.m_requestitemdescriptor = Need;
		Task.m_iRequestedAmount = ammount;
		if (Mag)
			Task.Mag = Mag;
		if(Task.Init())
		{
			m_aTaskMap.Insert(Task);
			Task.OnTaskFinished().Insert(OnTaskFinished);
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for tasks of provided entity
	static void GetCharTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		if (!Char)
			return;
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char))
			{
				tasks.Insert(task);
			}
			if(task.CharacterIsTarget(Char))
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if char has tasks that would require him to stay put. If he is looking for someone to do a delivery, unless he wants to do it, he would stay put so that people could find him and take said delivery, or if is target of del, so that delivery guy can find him.
	static bool GetCanAssignTask(IEntity Char)
	{
		array<ref SP_Task> assignedtasks = {};
		GetassignedTasks(Char, assignedtasks);
		if (!assignedtasks.IsEmpty())
			return false;
		array<ref SP_Task> ownedtasks = {};
		GetCharOwnedTasks(Char, ownedtasks);
		foreach (SP_Task task : ownedtasks)
		{
			//if (task.GetClassName() == SP_BountyTask || task.IsReserved())
				//continue;
			if (IsAssignable(task.GetClassName()))
			{
				return false;
			}
		}
		array<ref SP_Task> targettasks = {};
		GetCharTargetTasks(Char, targettasks);
		foreach (SP_Task task : targettasks)
		{
			//if (task.GetClassName() == SP_BountyTask)
				//continue;
			if (IsAssignable(task.GetClassName()))
			{
				return false;
			}
		}
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Assigning
	static void AssignMyTask(IEntity Char)
	{
		array<ref SP_Task> tasks = {};		
		GetCharOwnedTasks(Char, tasks);
		if (!tasks)
			return;
		
		foreach (SP_Task task : tasks)
		{
			if (IsAssignable(task.GetClassName()) && task.GetClassName() != SP_RetrieveTask && !task.IsOwnerAssigned() && !task.IsReserved() && task.GetState() == ETaskState.UNASSIGNED)
			{
				if (task.GetTimeLimit() > 3)
					continue;
				task.AssignOwner();
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Count of tasks of provided entity
	static int GetCharTaskCount(IEntity Char)
	{
		if (!Char)
			return -1;
		array<ref SP_Task> tasks = {};
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char))
			{
				tasks.Insert(task);
			}
			if(task.CharacterIsTarget(Char))
			{
				tasks.Insert(task);
			}
		}
		if (tasks.IsEmpty())
			return 0;
		return tasks.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	//Tasks owned by char that are not assigned
	static void GetUnassignedCharTasks(IEntity Char, IEntity Assignee, out array<ref SP_Task> tasks)
	{
		if (!Char)
			return;
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char) && !task.CharacterAssigned(Assignee))
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for amount of tasks
	static int GetInProgressTaskCount()
	{
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.GetState() == ETaskState.UNASSIGNED || task.GetState() == ETaskState.ASSIGNED)
			{
				tasks.Insert(task);
			}
		}
		return tasks.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for tasks of provided type that are related to the provided entity
	static void GetCharTasksOfSameType(IEntity Char,out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char) || task.CharacterIsTarget(Char))
			{
				if(task.GetClassName() == tasktype)
				{
					tasks.Insert(task);
				}
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for tasks of provided type that are related to the provided entity
	static void GetCharOwnedTasksOfSameType(IEntity Char,out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char))
			{
				if(task.GetClassName() == tasktype)
				{
					tasks.Insert(task);
				}
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter get ready to deliver tasks of character provided
	static void GetReadyToDeliver(IEntity Char,out array<ref SP_Task> tasks, IEntity Assignee)
	{
		array<ref SP_Task> Readytasks = {};
		{
			GetCharTasks(Char, Readytasks);
			if (Readytasks.IsEmpty())
				return;
			foreach (SP_Task task : Readytasks)
			{
				if (task.ReadyToDeliver(Char, Assignee))
					tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	static void GetCharRescueTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.GetClassName() == SP_RescueTask)
			{
				SP_RescueTask resctask = SP_RescueTask.Cast(task);
				if(resctask.GetCharsToResce().Contains(Char))
					tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	static void GetRescueTasks(out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.GetClassName() == SP_RescueTask)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//
	static void GetTasksCompletedBy(IEntity Char, out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : m_aCompletedTaskMap)
		{
			if(task.GetCompletionist() == Char)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for tasks of same type
	static void GetTasksOfSameType(out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.GetClassName() == tasktype)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	static void GetCharOwnedTasks(IEntity Char, out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}	
	//------------------------------------------------------------------------------------------------------------//
	static void GetAssignableTasks(IEntity Owner, out array<ref SP_Task> tasks,IEntity Assignee)
	{
		array <ref SP_Task> ownedtasks = {};
		GetCharOwnedTasks(Owner, ownedtasks);
		if (ownedtasks.IsEmpty())
		{
			return;
		}
		foreach (SP_Task task : ownedtasks)
		{
			if (task.CanBeAssigned(Owner, Assignee))
				tasks.Insert(task);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	static void GetCharTargetTasks(IEntity Char, out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Questlines
	void AssignInitTasks(IEntity Char)
	{
		array<ref SP_Task> qlines = {};		
		GetTasksOfSameType(qlines, SP_ChainedTask);
		bool assigned;
		if (!qlines.IsEmpty())
		{
			FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(Char.FindComponent(FactionAffiliationComponent));
			
			foreach (SP_Task task : qlines)
			{
				SP_ChainedTask chain = SP_ChainedTask.Cast(task);
				if (!chain)
					continue;
				if (chain.AssignOnInit && affcomp.GetAffiliatedFaction().GetFactionKey() == chain.key)
				{
					chain.AssignCharacter(Char);
					assigned = true;
				}
			}
		}
		
		if (!assigned)
		{
			GetGame().GetCallqueue().CallLater(AssignInitTasks, 1000, false, Char);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Garbage manager
	void ClearTasks()
	{
		int removed;
		array <ref SP_Task> toberemoved = new array <ref SP_Task>();
		for (int i = m_aTaskMap.Count() - 1; i >= 0; i--)
		{
			if (m_aTaskMap[i].MarkedForRemoval())
			{
				toberemoved.Insert(m_aTaskMap[i]);
				removed += 1;
			}
			if (m_aTaskMap[i].m_bFailOnTimeLimit && m_aTaskMap[i].GetState() == ETaskState.UNASSIGNED)
			{
				if (m_aTaskMap[i].GetTimeLimit() < 0 && m_aTaskMap[i].GetTimeLimit() != -1)
				{
					toberemoved.Insert(m_aTaskMap[i]);
					removed += 1;
				}
			}
		}
		for (int i = toberemoved.Count() - 1; i >= 0; i--)
		{
			m_aTaskMap[i].FailTask();
			m_aTaskMap.RemoveItem(toberemoved[i]);
		}
		Print(string.Format("Cleanup finished, %1 tasks got cleared", removed))
	}
	//------------------------------------------------------------------------------------------------------------//
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		SetEventMask(owner, EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	//------------------------------------------------------------------------------------------------------------//
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		//if (!m_bQuestInited)
			//return;
		m_fTaskClearTimer += timeSlice;
		if(m_fTaskClearTimer > m_fTaskClearTime)
		{
			m_fTaskClearTimer = 0;
			ClearTasks();
		}
		if (m_bShowDebug)
		{
			CreateDebug();
		}
		
		/*m_iMinTaskAmount = m_CharacterHolder.GetAliveCount() * m_fTaskPerCharacter;
		if (m_CharacterHolder.GetAliveCount() < m_iMinTaskAmount/m_fTaskPerCharacter)
			return;
		if (GetInProgressTaskCount() < m_iMinTaskAmount)
		{
			typename Task = m_aTaskSamples.GetRandomElement().GetClassName();
			CreateTask(Task);
		}
		else
		{*/
			m_fTaskRespawnTimer += timeSlice;
			if(m_fTaskRespawnTimer > m_fTaskGenTime)
			{
				typename Task;
				array <SP_Task> activetasks = GetActiveSamples();
				if (!activetasks || activetasks.IsEmpty())
				{
					Print("None of the tasks samples have been marked as enabled");
					return;
				}
				Task = activetasks.GetRandomElement().GetClassName();
				if(CreateTask(Task))
				{
					m_fTaskRespawnTimer = 0;
				}
				else
				{
					m_fTaskRespawnTimer -= 1;
				}
			}
		//}
	};
	//------------------------------------------------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		if (!GetGame().InPlayMode())
			return;
		//Init arrays
		if (!m_aTasksToSpawn)
		{
			return;
		}
		if (m_aTasksToSpawn.IsEmpty())
		{
			Print("No tasks configured in Request manager");
			return;
		}
		if(!m_aTaskMap)
			m_aTaskMap = new array<ref SP_Task>();
		if(!m_aCompletedTaskMap)
			m_aCompletedTaskMap = new array<ref SP_Task>();
		if(!m_aTaskSamples)
		{
			m_aTaskSamples = new array<ref SP_Task>();
		}
		//Tasks set to spawn and set them as samples to be used later
		if(m_aTaskSamples.IsEmpty())
		{
			foreach(SP_Task Task : m_aTasksToSpawn)
			{
				m_aTaskSamples.Insert(Task);
			}
		}
		//Get game mode and hook events
		m_CharacterHolder = new CharacterHolder ();
		//m_CharacterHolder.SetSpecialCharacterNameList(m_aSpecialCharacterNameList);
		if (!m_GameMode)
		{
			m_GameMode = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
			if (m_GameMode)
			{
				m_GameMode.GetOnControllableSpawned().Insert(m_CharacterHolder.OnNewCharacter);
				m_GameMode.GetOnControllableDestroyed().Insert(m_CharacterHolder.OnCharacterDeath);
				m_GameMode.GetOnControllableDeleted().Insert(m_CharacterHolder.OnCharacterDeleted);
			}
		}
		
		GetGame().GetCallqueue().CallLater(CreateChainedTasks, 1000);
	};
	//------------------------------------------------------------------------------------------------------------//
	static int GetCharCurrency(IEntity Char)
	{
		SCR_ChimeraCharacter ChimChar = SCR_ChimeraCharacter.Cast(Char);
		WalletEntity wallet = ChimChar.GetWallet();
		if (!wallet)
			return 0;
		int amount = wallet.GetCurrencyAmmount();
		return amount;
	};
	//------------------------------------------------------------------------------------------------------------//
	void CreateDebug()
	{
		if (m_CharacterHolder.GetAliveCount() == 0)
			return;
		if (!GetGame().GetCameraManager().CurrentCamera())
			return;
		vector Origin = GetGame().GetCameraManager().CurrentCamera().GetOrigin();
		array <ChimeraCharacter> AliveCharacters = {};
		m_CharacterHolder.GetAllAlive(AliveCharacters);
		foreach (ChimeraCharacter Owner : AliveCharacters)
		{
			if (vector.Distance(Origin, Owner.GetOrigin()) > 300)
				continue;
			array <ref SP_Task> OwnedTasks = {};
			GetCharOwnedTasks(Owner, OwnedTasks);
			string infoText2 = string.Format("CharName: %1\n Rank: %2\n Reputation: %3\nOwned Tasks: \n", SP_DialogueComponent.GetCharacterFirstName(Owner) + " " + SP_DialogueComponent.GetCharacterSurname(Owner), SP_DialogueComponent.GetCharacterRankName(Owner), SP_DialogueComponent.GetCharacterRep(Owner));
			foreach (SP_Task task : OwnedTasks)
			{
				string name;
				if (task.GetTarget())
					name = SP_DialogueComponent.GetCharacterFirstName(task.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(task.GetTarget());
				else
					name = SP_DialogueComponent.GetCharacterFirstName(task.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(task.GetOwner());
				string reservedstring;
				if (task.IsReserved())
					reservedstring = "RESERVED";
				else
					reservedstring = "UNRESERVED";
				string state = typename.EnumToString(ETaskState, task.GetState());
				float TimeLimit = task.GetTimeLimit();
				SP_RetrieveTask rtask = SP_RetrieveTask.Cast(task);
				if (rtask)
				{
					string itemname = string.Format("%1 x %2", rtask.m_iRequestedAmount, typename.EnumToString(ERequestRewardItemDesctiptor ,rtask.m_requestitemdescriptor));
					infoText2 = string.Format("%1 %2 Target : %3 | Retrieve item : %4 | Task State: %5 : %6 TimeLimit: %7\n", infoText2, task.GetClassName().ToString(), name, itemname , state, reservedstring, TimeLimit);
				}
				else
					infoText2 = string.Format("%1 %2 Target : %3  | Task State: %4 : %5 TimeLimit: %6\n", infoText2, task.GetClassName().ToString(), name, state, reservedstring, TimeLimit);
			}
			infoText2 = infoText2 + "Target of Tasks: \n";
			array <ref SP_Task> TargetedTasks = {};
			GetCharTargetTasks(Owner, TargetedTasks);
			foreach (SP_Task task : TargetedTasks)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(task.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(task.GetOwner());
				string reservedstring;
				if (task.IsReserved())
					reservedstring = "RESERVED";
				else
					reservedstring = "UNRESERVED";
				string state = typename.EnumToString(ETaskState, task.GetState());
				float TimeLimit = task.GetTimeLimit();
				infoText2 = string.Format("%1 %2 Owner : %3  | Task State: %4 : %5 TimeLimit: %6 \n", infoText2, task.GetClassName().ToString(), name, state, reservedstring, TimeLimit);
			}
			infoText2 = infoText2 + "Assigned Tasks: \n";
			array <ref SP_Task> AssignedTasks = {};
			GetassignedTasks(Owner, AssignedTasks);
			foreach (SP_Task task : AssignedTasks)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(task.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(task.GetOwner());
				string reservedstring;
				if (task.IsReserved())
					reservedstring = "RESERVED";
				else
					reservedstring = "UNRESERVED";
				string state = typename.EnumToString(ETaskState, task.GetState());
				float TimeLimit = task.GetTimeLimit();
				infoText2 = string.Format("%1 %2 Owner : %3  | Task State: %4 : %5 TimeLimit: %6  \n", infoText2, task.GetClassName().ToString(), name, state, reservedstring, TimeLimit);
			}
			int amount = GetCharCurrency(Owner);
			infoText2 = infoText2 + string.Format("Owned Currency: %1\n", amount);
			vector origin = Owner.GetOrigin();
			vector SphereOrig = Owner.GetOrigin();
			SphereOrig[1] = SphereOrig[1] + 5;
			
			AIControlComponent comp = AIControlComponent.Cast(Owner.FindComponent(AIControlComponent));
			if (!comp)
				return;
			
			AIAgent agent = comp.GetAIAgent();
			if (!agent)
				return;
			
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			if (!utility)
				return;
			
		
			SCR_AITaskPickupBehavior action = SCR_AITaskPickupBehavior.Cast(utility.FindActionOfType(SCR_AITaskPickupBehavior));
			
			if (action)
			{
				if (!action.PickedTask)
					continue;
				
				string name = SP_DialogueComponent.GetCharacterFirstName(action.PickedTask.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(action.PickedTask.GetOwner());
				
				infoText2 = infoText2 + string.Format("Heading towards %1's location to pick %2\n", name ,action.PickedTask.GetClassName().ToString());
				
				Shape.CreateSphere(Color.WHITE, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AIExecuteNavigateTaskBehavior Navaction = SCR_AIExecuteNavigateTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteNavigateTaskBehavior));
			
			if (Navaction)
			{
				if (!Navaction.PickedTask)
					continue;
				
				string name = SP_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetTarget());
				string Oname = SP_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetOwner());
				
				if (Navaction.PickedTask.IsOwnerAssigned())
					infoText2 = infoText2 + " | PERFORMING OWNED TASK | ";
				
				infoText2 = infoText2 + string.Format("Escorting %1 to %2's location\n", Oname ,name);
				
				Shape.CreateSphere(Color.BLUE, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AIExecuteDeliveryTaskBehavior Delaction = SCR_AIExecuteDeliveryTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteDeliveryTaskBehavior));
			if (Delaction)
			{
				if (!Delaction.PickedTask)
					continue;
				string name = SP_DialogueComponent.GetCharacterFirstName(Delaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Delaction.PickedTask.GetTarget());
				if (Delaction.PickedTask.IsOwnerAssigned())
					infoText2 = infoText2 + " | PERFORMING OWNED TASK | ";
				infoText2 = infoText2 + string.Format("Delivering Package to %1\n", name);
				Shape.CreateSphere(Color.GREEN, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AIExecuteBountyTaskBehavior Bountyaction = SCR_AIExecuteBountyTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteBountyTaskBehavior));
			if (Bountyaction)
			{
				if (!Bountyaction.PickedTask)
					continue;
				string name = SP_DialogueComponent.GetCharacterFirstName(Bountyaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Bountyaction.PickedTask.GetTarget());
				if (Bountyaction.PickedTask.IsOwnerAssigned())
					infoText2 = infoText2 + " | PERFORMING OWNED TASK | ";
				infoText2 = infoText2 + string.Format("Going after %1's bounty.\n", name);
				Shape.CreateSphere(Color.RED, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AIFollowBehavior followact = SCR_AIFollowBehavior.Cast(utility.FindActionOfType(SCR_AIFollowBehavior));
			if (followact)
			{
				if (!followact.Char)
					continue;
				string name = SP_DialogueComponent.GetCharacterFirstName(followact.Char) + " " + SP_DialogueComponent.GetCharacterSurname(followact.Char);
				infoText2 = infoText2 + string.Format("Following %1\n", name);
				Shape.CreateSphere(Color.ORANGE, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AIExecuteRetrieveTaskBehavior Retract = SCR_AIExecuteRetrieveTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteRetrieveTaskBehavior));
			if (Retract)
			{
				if (!Retract.PickedTask)
					continue;
				string name = SP_DialogueComponent.GetCharacterFirstName(Retract.PickedTask.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(Retract.PickedTask.GetOwner());
				infoText2 = infoText2 + string.Format("Following %1\n", name);
				Shape.CreateSphere(Color.BLACK, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AICombatComponent m_CombatComp = SCR_AICombatComponent.Cast(Owner.FindComponent(SCR_AICombatComponent));
			if (m_CombatComp)
			{
				SCR_InventoryStorageManagerComponent InvMan = SCR_InventoryStorageManagerComponent.Cast(Owner.FindComponent(SCR_InventoryStorageManagerComponent));
				BaseWeaponComponent weaponComp;
				BaseMagazineComponent magazineComp;
				BaseMagazineWell iMagWell;
				int muzzleId;
				weaponComp = m_CombatComp.GetCurrentWeapon();
			
				if (weaponComp)
				{
					int magamount;
					string magtype;
					if (weaponComp.GetCurrentMagazine())
					{
						iMagWell = weaponComp.GetCurrentMagazine().GetMagazineWell();
						magamount = m_CombatComp.GetMagazineCount(iMagWell.Type(), true);
					}
					if (iMagWell)
					{
						magtype = iMagWell.Type().ToString();
					}
					infoText2 = infoText2 + string.Format("Ammo Count: %1 of %2\n", magamount, magtype);
				}
			}
			
			DebugTextWorldSpace.Create(GetGame().GetWorld(), infoText2, DebugTextFlags.FACE_CAMERA | DebugTextFlags.IN_WORLD | DebugTextFlags.ONCE, origin[0], origin[1] + 4, origin[2], 0.1, 0xFFFFFFFF, Color.BLACK);
		}		
	}
};
//------------------------------------------------------------------------------------------------------------//
modded enum SCR_EArsenalItemType
{
	FOOD = 262200,
	DRINK = 262600,
	RADIO = 262800,
	COMPASS = 263000,
	FLASHLIGHT = 263500,
	MAP = 270000,
	CURRENCY = 280000,
	ARMOR = 290000,
	SLEEPING_PILLS = 300000,
};
//------------------------------------------------------------------------------------------------------------//
modded enum SCR_EArsenalItemMode
{
	GADGET = 128
};
[BaseContainerProps(configRoot: true), BaseContainerCustomCheckIntTitleField("m_bEnabled", "Request Data", "DISABLED - Request Data", 1)]
class SP_RequestData : SCR_BaseEntityCatalogData
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ERequestRewardItemDesctiptor))]
	ERequestRewardItemDesctiptor ItemDescriptor;
	
	[Attribute("1", desc: "Worth of entry")]
	protected int m_iWorth;
	
	protected SCR_EntityCatalogEntry m_EntryParent;
	protected ref Resource m_ItemResource;
	
	int GetWorth()
	{
		return m_iWorth;
	}
	
	ERequestRewardItemDesctiptor GetRequestDescriptor()
	{
		return ItemDescriptor;
	}
	//--------------------------------- Init Data ---------------------------------\\
	override void InitData(notnull SCR_EntityCatalogEntry entry)
	{
		m_EntryParent = entry;
		
		m_ItemResource = Resource.Load(m_EntryParent.GetPrefab());
	}
}
[BaseContainerProps(configRoot: true), BaseContainerCustomCheckIntTitleField("m_bEnabled", "Request Data", "DISABLED - Request Data", 1)]
class SP_RequestAmmoData : SP_RequestData
{
	[Attribute("0")]
	ref BaseMagazineWell MagType;
	
	BaseMagazineWell GetMagType(){return MagType;};
}
enum ERequestRewardItemDesctiptor
{
	CURRENCY = 0,
	WEAPON = 1,
	ARMOR = 2,
	HELMET = 3,
	CONSUMABLE = 4,
	AMMO = 5,
	FOOD_RAW = 6,
	FOOD_COOKED = 7,
	DRINK = 8,
	GADGET = 9,
	MORPHINE = 10,
	BANDAGE = 11,
	SLEEPING_PILLS = 12,
	BOTTLE = 13,
	COOKING_APPLIANCE = 14,
	EXPLOSIVE = 15,
	BACKPACK = 16,
	LOAD_BEARING_SYSTEM = 17,
	WEAPON_ATATCHMENT = 18,
	RADIO = 19,
	MAP = 20,
	FLASHLIGHT = 21,
	COMPASS = 22,
	BINOCULARS = 23,
	SIDEARM = 24,
	PANTS = 25,
	JACKET = 26,
	SHOES = 27,
}
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomEntityCatalogCatalog(EEntityCatalogType, "m_eEntityCatalogType", "m_aEntityEntryList", "m_aMultiLists")]
modded class SCR_EntityCatalog
{
	int GetWorthOfItem(ResourceName Item)
	{
		int worth = -1;
		SCR_EntityCatalogEntry entry = GetEntryWithPrefab(Item);
		SP_RequestData Data = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
		if (!Data)
		{
			SP_RequestAmmoData AmmoData = SP_RequestAmmoData.Cast(entry.GetEntityDataOfType(SP_RequestAmmoData));
			if (!AmmoData)
				return worth;
			worth = AmmoData.GetWorth();
			return worth;
		}
		worth = Data.GetWorth();
		return worth;
	}
	void GetRequestItems(int Descriptor, BaseMagazineComponent mag, out array <SCR_EntityCatalogEntry>  correctentries)
	{
		
		correctentries = {};		
		foreach (SCR_EntityCatalogEntry entityEntry: m_aEntityEntryList)
		{
			SP_RequestData Data = SP_RequestData.Cast(entityEntry.GetEntityDataOfType(SP_RequestData));
			if (Data)
			{
				if (Data.GetRequestDescriptor() == Descriptor)
				{
					correctentries.Insert(entityEntry);
					continue;
				}
			}
			SP_RequestAmmoData AmmoData = SP_RequestAmmoData.Cast(entityEntry.GetEntityDataOfType(SP_RequestAmmoData));
			if (AmmoData)
			{
				if (AmmoData.GetRequestDescriptor() == Descriptor)
				{
					if (mag)
					{
						if (mag.GetMagazineWell().ClassName() != AmmoData.GetMagType().ClassName())
							continue;
					}
					correctentries.Insert(entityEntry);
					continue;
				}
			}
		}
	}
}