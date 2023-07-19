class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
class SP_RequestManagerComponent : ScriptComponent
{
	[Attribute()]
	float m_fTaskGenTime;
	
	[Attribute(defvalue: "20", desc:"Tasks will be continiusly created until this number is met, after that askRespawnTimer will have to pass for a task to be created")]
	int m_iMinTaskAmount;
	
	[Attribute(defvalue: "3", desc: "Max amount of tasks a character can be requesting at the same time")]
	int m_fTaskPerCharacter;
	
	[Attribute(defvalue: "2", desc: "Max amount of tasks of sametype a character can be requesting at the same time")]
	int m_fTaskOfSameTypePerCharacter;
	
	[Attribute()]
	ref array<ref SP_Task> TasksToSawn;
	
	static ref array<ref SP_Task> TaskSamples = null;
	
	[Attribute(defvalue: "60", desc: "Task garbage mamager kinda. Completed task are added to their own list, failed tasks are deleted")]
	float m_fTaskClearTime;
	
	protected float m_fTaskRespawnTimer;
	protected float m_fTaskClearTimer;
	
	SP_GameMode m_GameMode;
	protected CharacterHolder Chars;
	//------------------------------------------------------------------------------------------------------------//
	static ref array<ref SP_Task> TaskMap = null;
	static ref array<ref SP_Task> CompletedTaskMap = null;
	//------------------------------------------------------------------------------------------------------------//
	protected ref ScriptInvoker s_OnTaskComplete = new ref ScriptInvoker();
	protected ref ScriptInvoker s_OnTaskCreated = new ref ScriptInvoker();
	//------------------------------------------------------------------------------------------------------------//
	void ~SP_RequestManagerComponent(){TaskMap.Clear();TaskMap.Clear();TaskSamples.Clear();};
	//------------------------------------------------------------------------------------------------------------//
	ScriptInvoker OnTaskComplete()
	{
		return s_OnTaskComplete;
	}
	ScriptInvoker OnTaskCreated()
	{
		return s_OnTaskCreated;
	}
	void OnNewChar(IEntity Char)
	{
		Chars.InserCharacter(ChimeraCharacter.Cast(Char));
	}
	void OnCharDeath(IEntity Char)
	{
		Chars.CharIsDead(ChimeraCharacter.Cast(Char));
	}
	override void EOnInit(IEntity owner)
	{
		if(!TaskMap)
			TaskMap = new array<ref SP_Task>();
		if(!CompletedTaskMap)
			CompletedTaskMap = new array<ref SP_Task>();
		if(!TaskSamples)
		{
			TaskSamples = new array<ref SP_Task>();
		}
		if(TaskSamples.Count() == 0)
		{
			foreach(SP_Task Task : TasksToSawn)
			{
				TaskSamples.Insert(Task);
			}
		}
		if (!m_GameMode)
		{
			m_GameMode = SP_GameMode.Cast(GetGame().GetGameMode());
			if (m_GameMode)
			{
				m_GameMode.GetOnControllableDestroyed().Insert(UpdateCharacterTasks);
				m_GameMode.GetOnControllableSpawned().Insert(OnNewChar);
				m_GameMode.GetOnControllableDestroyed().Insert(OnCharDeath);
			}
		}
	}
	SP_Task GetTaskSample(typename tasktype)
	{
		foreach(SP_Task Task : TaskSamples)
		{
			if(Task.GetClassName() == tasktype)
			{
				return Task;
			}
		}
		return null;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CharHasTask(IEntity Char)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CharIsTarget(IEntity Char)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	void UpdateCharacterTasks(IEntity Char, IEntity Instigator)
	{
		if(Char)
		{
			foreach (SP_Task task : TaskMap)
			{
				if(task.CharacterIsOwner(Char) == true)
				{
					task.UpdateState();
				}
				if(task.CharacterIsTarget(Char) == true)
				{
					task.UpdateState();
				}
			}
		}
		if(Instigator)
		{
			foreach (SP_Task task : TaskMap)
			{
				if(task.CharacterIsOwner(Instigator) == true)
				{
					task.UpdateState();
				}
				if(task.CharacterIsTarget(Instigator) == true)
				{
					task.UpdateState();
				}
			}
		}
		
	}
	//------------------------------------------------------------------------------------------------------------//
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
			TaskMap.Insert(Task);
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetCharTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	int GetInProgressTaskCount()
	{
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetState() == ETaskState.UNASSIGNED || task.GetState() == ETaskState.ASSIGNED)
			{
				tasks.Insert(task);
			}
		}
		return tasks.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetCharTasksOfSameType(IEntity Char,out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				if(task.GetClassName() == tasktype)
				{
					tasks.Insert(task);
				}
				
			}
			if(task.CharacterIsTarget(Char) == true)
			{
				if(task.GetClassName() == tasktype)
				{
					tasks.Insert(task);
				}
				
			}
		}
	}
	void GetCharRescueTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetClassName() == SP_RescueTask)
			{
				SP_RescueTask resctask = SP_RescueTask.Cast(task);
				if(resctask.GetCharsToResc().Contains(Char))
					tasks.Insert(task);
			}
		}
	}
	void GetRescueTasks(out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetClassName() == SP_RescueTask)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetTasksOfSameType(out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetClassName() == tasktype)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetCharTargetTasks(IEntity Char, out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void ClearTasks()
	{
		int removed;
		for (int i = TaskMap.Count() - 1; i >= 0; i--)
		{
			if (TaskMap[i].GetState() == ETaskState.FAILED) 
			{
				TaskMap.Remove(i);
				removed += 1;
			}
			if (TaskMap[i].GetState() == ETaskState.COMPLETED) 
			{
				OnTaskCompleted(TaskMap[i]);
				removed += 1;
			}
		}
		Print(string.Format("Cleanup finished, %1 tasks got cleared", removed))
		/*foreach (SP_Task task : TaskMap)
	    {
				if (task.GetState() == ETaskState.FAILED) 
				{
					TaskMap.Remove(TaskMap.Find(task));
					c--;
					continue;
				}
				if (task.GetState() == ETaskState.COMPLETED) 
				{
					OnTaskCompleted(task);
				}
				i++;
			}
		*/
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
		if (Chars.GetAliveCount() < m_iMinTaskAmount/m_fTaskPerCharacter)
			return;
		if (GetInProgressTaskCount() < m_iMinTaskAmount)
		{
			typename Task = TaskSamples.GetRandomElement().GetClassName();
			CreateTask(Task);
		}
		else
		{
			m_fTaskRespawnTimer += timeSlice;
			if(m_fTaskRespawnTimer > m_fTaskGenTime)
			{
				typename Task;
				Task = TaskSamples.GetRandomElement().GetClassName();
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
		m_fTaskClearTimer += timeSlice;
		if(m_fTaskClearTimer > m_fTaskClearTime)
		{
			m_fTaskClearTimer = 0;
			ClearTasks();
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	void OnTaskCompleted(SP_Task Task)
	{
		TaskMap.RemoveItem(Task);
		CompletedTaskMap.Insert(Task);
		s_OnTaskComplete.Invoke(Task, Task.GetCompletionist());
	};
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
};
//------------------------------------------------------------------------------------------------------------//
modded enum SCR_EArsenalItemMode
{
	GADGET = 128
	
};
//------------------------------------------------------------------//
class CharacterHolder : ScriptAndConfig
{
	ref array <ChimeraCharacter> AliveCharacters = new ref array <ChimeraCharacter>();
	ref array <ChimeraCharacter> DeadCharacters = new ref array <ChimeraCharacter>();
	void InserCharacter(ChimeraCharacter Char)
	{
		AliveCharacters.Insert(Char);
	}
	void CharIsDead(ChimeraCharacter Char)
	{
		AliveCharacters.RemoveItem(Char);
		DeadCharacters.Insert(Char);
	}
	int GetAliveCount()
	{
		return AliveCharacters.Count();
	}
	bool GetUnitOfFaction(Faction fact, ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			mychar = AliveCharacters.GetRandomElement();
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if(Aff.GetAffiliatedFaction() == fact)
			{
				return true;
			}
				
		}
		return false;
	}
	bool GetRandomUnit()
	{
	
	}
}