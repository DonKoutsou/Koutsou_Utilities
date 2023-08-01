//////////////////////////////////////////////////////////////
//---------------------Request Manager---------------------//
class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
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
	
	//Tasks samples to set up settings for all different tasks
	static ref array<ref SP_Task> m_aTaskSamples = null;
	
	//Garbage Manager
	[Attribute(defvalue: "60", desc: "Task garbage mamager kinda. Completed task are added to their own list, failed tasks are deleted")]
	float m_fTaskClearTime;
	
	protected float m_fTaskRespawnTimer;
	protected float m_fTaskClearTimer;
	
	SP_GameMode m_GameMode;
	protected ref CharacterHolder m_CharacterHolder = new CharacterHolder();
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
	//Destructor
	void ~SP_RequestManagerComponent(){if (m_aCompletedTaskMap)m_aCompletedTaskMap.Clear();if (m_aTaskMap)m_aTaskMap.Clear();if (m_aTaskSamples)m_aTaskSamples.Clear();};
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
	//--------------------------------------------------------------------//
	CharacterHolder GetCharacterHolder()
	{
		return m_CharacterHolder;
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
	SP_Task GetTaskSample(typename tasktype)
	{
		foreach(SP_Task Task : m_aTaskSamples)
		{
			if(Task.GetClassName() == tasktype)
			{
				return Task;
			}
		}
		return null;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if entity provided has tasks
	bool CharHasTask(IEntity Char)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if entity provided is target of any of the tasks
	bool CharIsTarget(IEntity Char)
	{
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Create tasks of type
	bool CreateTask(typename TaskType)
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
	bool CreateChainedTask(IEntity Owner, array <ref SP_Task> InTasks)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(m_GameMode.GetDialogueComponent());
		SP_ChainedTask Task;
		if (Owner)
			Task.TaskOwner = Owner;
		if (!InTasks.IsEmpty())
			Task.SetTasklist(InTasks);
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
	void GetCharTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		if (!Char)
			return;
		foreach (SP_Task task : m_aTaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for amount of tasks
	int GetInProgressTaskCount()
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
	void GetCharTasksOfSameType(IEntity Char,out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : m_aTaskMap)
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
	//------------------------------------------------------------------------------------------------------------//
	void GetCharRescueTasks(IEntity Char,out array<ref SP_Task> tasks)
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
	void GetRescueTasks(out array<ref SP_Task> tasks)
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
	//Getter for tasks of same type
	void GetTasksOfSameType(out array<ref SP_Task> tasks, typename tasktype)
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
	void GetCharTargetTasks(IEntity Char, out array<ref SP_Task> tasks)
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
	void ClearTasks()
	{
		int removed;
		array <SP_Task> toberemoved = new array <SP_Task>();
		for (int i = m_aTaskMap.Count() - 1; i >= 0; i--)
		{
			if (m_aTaskMap[i].GetState() == ETaskState.FAILED) 
			{
				toberemoved.Insert(m_aTaskMap[i]);
				removed += 1;
			}
			if (m_aTaskMap[i].GetState() == ETaskState.COMPLETED) 
			{
				OnTaskCompleted(m_aTaskMap[i]);
				removed += 1;
			}
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
		m_iMinTaskAmount = m_CharacterHolder.GetAliveCount() * m_fTaskPerCharacter;
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
		m_fTaskClearTimer += timeSlice;
		if(m_fTaskClearTimer > m_fTaskClearTime)
		{
			m_fTaskClearTimer = 0;
			ClearTasks();
		}
	};
	override void EOnInit(IEntity owner)
	{
		//Init arrays
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
		if(m_aTaskSamples.Count() == 0)
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
	ARMOR = 290000,
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
	private ref array <ChimeraCharacter> AliveCharacters;
	private ref array <ChimeraCharacter> DeadCharacters;
	//------------------------------------------------------------------------------------------------------------//
	void InserCharacter(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		AliveCharacters.Insert(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	void CharIsDead(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		AliveCharacters.RemoveItem(Char);
		DeadCharacters.Insert(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	void CharIsDel(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		if (AliveCharacters.Contains(Char))
			AliveCharacters.RemoveItem(Char);
		if (DeadCharacters.Contains(Char))
			DeadCharacters.RemoveItem(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	int GetAliveCount()
	{
		return AliveCharacters.Count();
	}
	ChimeraCharacter GetRandomDeadOfFaction(Faction fact)
	{
		ChimeraCharacter mychar;
		for (int i = 0; i < 10; i++)
		{
			mychar = AliveCharacters.GetRandomElement();
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
	bool GetUnitOfFaction(Faction fact, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			mychar = AliveCharacters.GetRandomElement();
			if (!mychar)
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
	//------------------------------------------------------------------------------------------------------------//
	bool GetCharOfRank(SCR_CharacterRank rank, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			mychar = AliveCharacters.GetRandomElement();
			if (!mychar)
				continue;
			SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(mychar.FindComponent(SCR_CharacterRankComponent));
			if (!Rankcomp)
				continue;
			if(Rankcomp.GetCharacterRankName(mychar) == rank)
			{
				return true;
			}
		}
		mychar = null;
		return false;	
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit far form provided characters location
	bool GetFarUnit(ChimeraCharacter mychar, float mindistance, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			FarChar = AliveCharacters.GetRandomElement();
			float dist = vector.Distance(FarChar.GetOrigin(), mychar.GetOrigin());
			if (mindistance > dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit far form provided location
	bool GetFarUnit(vector pos, float mindistance, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			FarChar = AliveCharacters.GetRandomElement();
			float dist = vector.Distance(FarChar.GetOrigin(), pos);
			if (mindistance < dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit of specified faction far form provided character
	bool GetFarUnitOfFaction(ChimeraCharacter mychar, float mindistance, Faction fact, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			GetUnitOfFaction(fact, FarChar);
			if (!FarChar)
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), mychar.GetOrigin());
			if (mindistance < dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool GetRandomUnit(out ChimeraCharacter mychar)
	{
		mychar = AliveCharacters.GetRandomElement();
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
	}
	
	void ~CharacterHolder()
	{
		AliveCharacters.Clear();
		DeadCharacters.Clear();
	}
	
}