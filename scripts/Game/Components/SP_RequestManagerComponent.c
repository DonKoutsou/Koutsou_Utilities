//////////////////////////////////////////////////////////////
//---------------------Request Manager---------------------//
class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
[BaseContainerProps(configRoot: true)]
class SP_RequestManagerComponent : ScriptComponent
{
	[Attribute(defvalue : "3", desc: "Cooldown for task generation")]
	float m_fTaskGenTime;
	
	protected int m_iMinTaskAmount;
	
	[Attribute(defvalue: "3", desc: "Max amount of tasks a character can be requesting at the same time")]
	protected int m_fTaskPerCharacter;
	
	[Attribute(defvalue: "2", desc: "Max amount of tasks of sametype a character can be requesting at the same time")]
	protected int m_fTaskOfSameTypePerCharacter;
	
	[Attribute(desc: "Type of tasks that will be created by request manager. Doesent stop from creating different type of task wich doesent exist here.")]
	ref array<ref SP_Task> m_aTasksToSpawn;
	
	[Attribute(desc: "Type of tasks that will be created by request manager. Doesent stop from creating different type of task wich doesent exist here.")]
	ref array<ref SP_ChainedTask> m_aQuestlines;
	
	[Attribute()]
	ref array<string> m_aSpecialCars;
	
	bool m_bQuestInited;
	
	//Tasks samples to set up settings for all different tasks
	static ref array<ref SP_Task> m_aTaskSamples = null;
	
	//Garbage Manager
	[Attribute(defvalue: "60", desc: "Task garbage mamager kinda. Completed task are added to their own list, failed tasks are deleted")]
	float m_fTaskClearTime;
	
	[Attribute(defvalue:"0")]
	bool m_bShowDebug;
	
	protected float m_fTaskRespawnTimer;
	protected float m_fTaskClearTimer;
	protected float m_fDebugTimer;
	SP_GameMode m_GameMode;
	
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
	ScriptInvoker OnTaskFail()
	{
		return s_OnTaskFailed;
	}
	ScriptInvoker OnTaskCreated()
	{
		return s_OnTaskCreated;
	}
	private event void OnNewChar(IEntity Char)
	{
		m_CharacterHolder.InserCharacter(ChimeraCharacter.Cast(Char));
	}
	private event void OnCharDeath(IEntity Char)
	{
		m_CharacterHolder.CharIsDead(ChimeraCharacter.Cast(Char));
	}
	private event void OnCharDel(IEntity Char)
	{
		m_CharacterHolder.CharIsDel(ChimeraCharacter.Cast(Char));
	}
	event void OnTaskCompleted(SP_Task Task)
	{
		m_aTaskMap.RemoveItem(Task);
		m_aCompletedTaskMap.Insert(Task);
		OnTaskComplete().Invoke(Task, Task.GetCompletionist());
	};
	event void OnTaskFailed(SP_Task Task)
	{
		m_aTaskMap.RemoveItem(Task);
		OnTaskFail().Invoke(Task, Task.GetCompletionist());
	};
	void OnTaskFinished(SP_Task task)
	{
		task.OnTaskFinished().Remove(OnTaskFinished);
		ETaskState state = task.GetState();
		if (state == ETaskState.COMPLETED)
			OnTaskCompleted(task);
		if (state == ETaskState.FAILED)
			OnTaskFailed(task);
	}
	int GetTasksPerCharacter()
	{
		return m_fTaskPerCharacter;
	}
	int GetTasksOfSameTypePerCharacter()
	{
		return m_fTaskOfSameTypePerCharacter;
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
	static bool CharIsPickingTask(IEntity Char)
	{
		if (!Char)
			return false;
		AIControlComponent comp = AIControlComponent.Cast(Char.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
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
	private bool CreateTask(typename TaskType)
	{
		if(!TaskType)
		{
			return false;
		}
		if (!GetTaskSample(TaskType).m_bEnabled)
			return false;
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
	private bool CreateChainedTasks()
	{
		bool anyfailed;
		foreach (SP_ChainedTask qLine : m_aQuestlines)
		{
			if (m_aTaskMap.Contains(qLine))
				continue;
			SP_DialogueComponent Diag = SP_DialogueComponent.Cast(m_GameMode.GetDialogueComponent());
			qLine.Init();
			m_aTaskMap.Insert(qLine);
			qLine.OnTaskFinished().Insert(OnTaskFinished);
		}
		m_bQuestInited = true;
		return true;
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
			if (task.GetClassName() == SP_BountyTask)
				continue;
			if (IsAssignable(task.GetClassName()))
			{
				return false;
			}
		}
		array<ref SP_Task> targettasks = {};
		GetCharTargetTasks(Char, targettasks);
		foreach (SP_Task task : targettasks)
		{
			if (task.GetClassName() == SP_BountyTask)
				continue;
			if (IsAssignable(task.GetClassName()))
			{
				return false;
			}
		}
		return true;
	}
	static void AssignMyTask(IEntity Char)
	{
		array<ref SP_Task> tasks = {};		
		GetCharOwnedTasks(Char, tasks);
		if (!tasks)
			return;
		
		foreach (SP_Task task : tasks)
		{
			if (IsAssignable(task.GetClassName()) && task.GetClassName() != SP_BountyTask && !task.IsOwnerAssigned() && !task.IsReserved())
			{
				if (task.AssignOwner())
					return;
			}
		}
	}
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
				if(resctask.GetCharsToResc().Contains(Char))
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
	void AssignInitTasks(IEntity Char)
	{
		if (m_aQuestlines.IsEmpty())
			return;
		FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(Char.FindComponent(FactionAffiliationComponent));
		foreach (SP_ChainedTask chain : m_aQuestlines)
		{
			if (chain.AssignOnInit && affcomp.GetAffiliatedFaction().GetFactionKey() == chain.key)
			{
				chain.AssignCharacter(Char);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
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
		}
		for (int i = toberemoved.Count() - 1; i >= 0; i--)
		{
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
	void AssignATask()
	{
		ChimeraCharacter Assignee;
		if (!m_CharacterHolder.GetRandomUnit(Assignee))
			return;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Assignee.GetDamageManager());
		if (dmg.GetIsUnconscious() || dmg.IsDestroyed())
			return;
		if (!GetCanAssignTask(Assignee) && !CharHasOwnAssigned(Assignee))
		{
			AssignMyTask(Assignee);
			return;
		}
			
		
		FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		ChimeraCharacter CloseChar;
		if (!m_CharacterHolder.GetUnitOfAnyFriendlyFaction(affcomp.GetAffiliatedFaction(), CloseChar))
			return;
		if (Assignee == CloseChar)
			return;
		array <ref SP_Task> tasks = {};
		foreach (SP_Task task : m_aTaskSamples)
		{
			if (task.m_bAssignable)
				GetCharOwnedTasksOfSameType(CloseChar, tasks, task.GetClassName());
		}
		if (tasks.IsEmpty())
			return;
		for (int i = tasks.Count() - 1; i >= 0; i--)
		{
			ref SP_Task mytask = tasks.GetRandomElement();
			if (!mytask)
				continue;
			if (mytask.GetTarget() == Assignee)
				continue;
			if (mytask.IsReserved())
				continue;
			AIControlComponent comp = AIControlComponent.Cast(Assignee.FindComponent(AIControlComponent));
			if (!comp)
				return;
			AIAgent agent = comp.GetAIAgent();
			if (!agent)
				return;
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			if (!utility)
				return;
			SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
			if (!group)
				return;
			/*if (group.GetAgentsCount() > 1)
			{
				group.RemoveAgent(agent);
				Resource groupbase = Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et");
				EntitySpawnParams myparams = EntitySpawnParams();
				myparams.TransformMode = ETransformMode.WORLD;
				myparams.Transform[3] = Assignee.GetOrigin();
				SCR_AIGroup newgroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupbase, GetGame().GetWorld(), myparams));
				newgroup.AddAgent(agent);
				newgroup.AddWaypoint(group.GetCurrentWaypoint());
			}
			//else
				//group.CompleteWaypoint(group.GetCurrentWaypoint());*/
			SCR_AITaskPickupBehavior action = new SCR_AITaskPickupBehavior(utility, null, mytask);
			utility.AddAction(action);
			mytask.SetReserved(Assignee);
			//if (tasks.GetRandomElement().AssignCharacter(Assignee))
			
			return;
		}
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
		
		m_iMinTaskAmount = m_CharacterHolder.GetAliveCount();
		if (m_CharacterHolder.GetAliveCount() < m_iMinTaskAmount/m_fTaskPerCharacter)
			return;
		if (GetInProgressTaskCount() < m_iMinTaskAmount)
		{
			typename Task = m_aTaskSamples.GetRandomElement().GetClassName();
			CreateTask(Task);
		}
		else
		{
			m_fTaskRespawnTimer += timeSlice;
			if(m_fTaskRespawnTimer > m_fTaskGenTime)
			{
				typename Task;
				Task = m_aTaskSamples.GetRandomElement().GetClassName();
				if(CreateTask(Task))
				{
					m_fTaskRespawnTimer = 0;
				}
				else
				{
					m_fTaskRespawnTimer -= 1;
				}
			}
		}
		
		AssignATask();
	};
	override void EOnInit(IEntity owner)
	{
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
		if (!m_GameMode)
		{
			m_GameMode = SP_GameMode.Cast(GetGame().GetGameMode());
			if (m_GameMode)
			{
				m_GameMode.GetOnControllableSpawned().Insert(OnNewChar);
				m_GameMode.GetOnControllableDestroyed().Insert(OnCharDeath);
				m_GameMode.GetOnControllableDeleted().Insert(OnCharDel);
			}
		}
		m_CharacterHolder = new CharacterHolder ();
		m_CharacterHolder.m_aSpecialCars.Copy(m_aSpecialCars);
		GetGame().GetCallqueue().CallLater(CreateChainedTasks, 1000);
	};
	static int GetCharCurrency(IEntity Char)
	{
		SCR_ChimeraCharacter ChimChar = SCR_ChimeraCharacter.Cast(Char);
		int amount = ChimChar.GetWallet().GetCurrencyAmmount();
		return amount;
	};
	void CreateDebug()
	{
		if (m_CharacterHolder.GetAliveCount() == 0)
			return;
		if (!GetGame().GetCameraManager().CurrentCamera())
			return;
		vector Origin = GetGame().GetCameraManager().CurrentCamera().GetOrigin();
		foreach (ChimeraCharacter Owner : m_CharacterHolder.GetAllAlive())
		{
			//if (vector.Distance(Origin, Owner.GetOrigin()) > 100)
				//continue;
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
				string state = typename.EnumToString(ETaskState, task.GetState());
				infoText2 = string.Format("%1 %2 Target : %3 %4 | Task State: %5 \n", infoText2, task.GetClassName().ToString(), name, reservedstring, state);
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
				string state = typename.EnumToString(ETaskState, task.GetState());
				infoText2 = string.Format("%1 %2 Owner : %3 %4 | Task State: %5  \n", infoText2, task.GetClassName().ToString(), name, reservedstring, state);
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
				string state = typename.EnumToString(ETaskState, task.GetState());
				infoText2 = string.Format("%1 %2 Owner : %3 %4 | Task State: %5   \n", infoText2, task.GetClassName().ToString(), name, reservedstring, state);
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
				
				infoText2 = infoText2 + string.Format("Heading towards %1's location to pick %2", name ,action.PickedTask.GetClassName().ToString());
				
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
				
				infoText2 = infoText2 + string.Format("Escorting %1 to %2's location", Oname ,name);
				
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
				infoText2 = infoText2 + string.Format("Delivering Package to %1", name);
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
				infoText2 = infoText2 + string.Format("Going after %1's bounty.", name);
				Shape.CreateSphere(Color.RED, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			SCR_AIFollowBehavior followact = SCR_AIFollowBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteBountyTaskBehavior));
			if (followact)
			{
				if (!followact.Char)
					continue;
				string name = SP_DialogueComponent.GetCharacterFirstName(followact.Char) + " " + SP_DialogueComponent.GetCharacterSurname(followact.Char);
				infoText2 = infoText2 + string.Format("Following %1", name);
				Shape.CreateSphere(Color.ORANGE, ShapeFlags.DEFAULT | ShapeFlags.ONCE, SphereOrig, 1);
			}
			/*int highlightRegion = -1;
			float highlightDist = -1;
			vector textMat[4];
			GetGame().GetWorld().GetCurrentCamera(textMat);
			vector camDir = textMat[2];
			vector camPos = textMat[3];
			vector regionPos = Owner.GetOrigin();
			regionPos[1] = regionPos[1] + 4;
			vector intersectScreenDiff = regionPos.InvMultiply4(textMat);
			intersectScreenDiff[2] = 0;
					
			float distScale = vector.Distance(camPos, regionPos) * 0.1;
			distScale = Math.Clamp(distScale, 0.5, 5);
					
			float distToCircle = intersectScreenDiff.Length() / distScale;
			if (distToCircle < 0.75 && (distToCircle < highlightDist || highlightDist == -1))
			{
				highlightDist = distToCircle;
			}
					
			distScale = vector.Distance(camPos, regionPos) * 0.1;
			distScale = Math.Clamp(distScale, 0.5, 3);
					
			textMat[3] = textMat[1] * 0.05 * distScale + regionPos;
			//SCR_AITaskPickupBehavior*/
			DebugTextWorldSpace.Create(GetGame().GetWorld(), infoText2, DebugTextFlags.FACE_CAMERA | DebugTextFlags.IN_WORLD | DebugTextFlags.ONCE, origin[0], origin[1] + 4, origin[2], 0.1, 0xFFFFFFFF, Color.BLACK);
			//CreateSimpleText(infoText2, textMat, 0.1, Color.WHITE, ShapeFlags.ONCE, null, 1, true, Color.BLACK);
			//Shape.Create(
			//CreateSimpleText(name, mat, SCR_UnitDisplaySettings.s_fShapeTextSize, textcolor, ShapeFlags.ONCE, null, 1, false);
		}		
	}
};
//------------------------------------------------------------------------------------------------------------//
modded enum EWeaponType
{
	WT_KNIFE
}
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
//------------------------------------------------------------------//
//Class to contain all characters
class CharacterHolder : ScriptAndConfig
{
	static private ref array <ChimeraCharacter> AliveCharacters;
	static private ref array <ChimeraCharacter> DeadCharacters;

	static ref array<string> m_aSpecialCars;
	
	
	array <ChimeraCharacter> GetAllAlive()
	{
		return AliveCharacters;
	}
	static void InserCharacter(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		if (m_aSpecialCars.Contains(Char.GetName()))
			return;
		AliveCharacters.Insert(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	static void CharIsDead(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		AliveCharacters.RemoveItem(Char);
		DeadCharacters.Insert(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	static void CharIsDel(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		if (AliveCharacters.Contains(Char))
			AliveCharacters.RemoveItem(Char);
		if (DeadCharacters.Contains(Char))
			DeadCharacters.RemoveItem(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	static int GetAliveCount()
	{
		return AliveCharacters.Count();
	}
	static ChimeraCharacter GetRandomDeadOfFaction(Faction fact)
	{
		ChimeraCharacter mychar;
		if (DeadCharacters.IsEmpty())
			return null;
		for (int i = 0; i < 10; i++)
		{
			mychar = DeadCharacters.GetRandomElement();
			if (!mychar)
				continue;
			
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if (!Aff)
				continue;
			if(Aff.GetAffiliatedFaction() == fact)
			{
				return mychar;
			}
		}
		mychar = null;
		return mychar;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool GetUnitOfFaction(Faction fact, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(mychar))
				continue;
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if (!Aff)
				continue;
			if(Aff.GetAffiliatedFaction() == fact)
			{
				return true;
			}
		}
		mychar = null;
		return false;
	}
	static bool GetUnitOfAnyFriendlyFaction(Faction fact, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(mychar))
				continue;
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if (!Aff)
				continue;
			if(Aff.GetAffiliatedFaction().IsFactionFriendly(fact))
			{
				return true;
			}
		}
		mychar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool GetCharOfRank(SCR_CharacterRank rank, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(mychar))
				continue;
			SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(mychar.FindComponent(SCR_CharacterRankComponent));
			if (!Rankcomp)
				continue;
			if(Rankcomp.GetCharacterRankName(mychar) == rank.ToString())
			{
				return true;
			}
		}
		mychar = null;
		return false;	
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit far form provided characters location
	static bool GetFarUnit(ChimeraCharacter mychar, float mindistance, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(FarChar))
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), mychar.GetOrigin());
			if (mindistance > dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit far form provided location
	static bool GetFarUnit(vector pos, float mindistance, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(FarChar))
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), pos);
			if (mindistance < dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit close provided location
	static bool GetCloseUnit(vector pos, float maxdistance, out ChimeraCharacter CloseChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(CloseChar))
				continue;
			float dist = vector.Distance(CloseChar.GetOrigin(), pos);
			if (maxdistance > dist)
				return true;
		}
		CloseChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit of specified faction far form provided character
	static bool GetFarUnitOfFaction(ChimeraCharacter mychar, float mindistance, Faction fact, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetUnitOfFaction(fact, FarChar))
				continue;
			if (FarChar == mychar)
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), mychar.GetOrigin());
			if (mindistance < dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit of specified faction clsoe to provided character
	static bool GetCloseUnitOfFaction(ChimeraCharacter mychar, float maxdistance, Faction fact, out ChimeraCharacter CloseChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetUnitOfFaction(fact, CloseChar))
				continue;
			if (CloseChar == mychar)
				continue;
			float dist = vector.Distance(CloseChar.GetOrigin(), mychar.GetOrigin());
			if (maxdistance > dist)
				return true;
		}
		CloseChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool GetRandomUnit(out ChimeraCharacter mychar)
	{
		if (AliveCharacters.IsEmpty())
			return false;
		mychar = AliveCharacters.GetRandomElement();
		if (m_aSpecialCars.Contains(mychar.GetName()))
			return false;
		if (mychar.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup())
			if (m_aSpecialCars.Contains(mychar.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetName()))
				return false;
		if (SCR_EntityHelper.IsPlayer(mychar))
			return false;
		if (mychar)
			return true;
		return false;
	}
	/*bool GetStrandedGroup(out SCR_AIGroup group)
	{
		for (int i = 0; i < 10; i++)
		{
			group = SCR_AIGroup.Cast(AliveCharacters.GetRandomElement().GetParent());
			if (!group)
				continue;
		
			if (CheckForCharacters(200, group.GetOrigin()))
				group = null;
			if (group)
				return true;
		}
		return false;
	}*/
	void CharacterHolder()
	{
		if (!AliveCharacters)
			AliveCharacters = new ref array <ChimeraCharacter>();
		if (!DeadCharacters)
			DeadCharacters = new ref array <ChimeraCharacter>();
		if (!m_aSpecialCars)
			m_aSpecialCars = new array<string>();
	}
	
	void ~CharacterHolder()
	{
		AliveCharacters.Clear();
		DeadCharacters.Clear();
		m_aSpecialCars.Clear();
	}
	void Init();
}