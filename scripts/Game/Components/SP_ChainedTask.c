[BaseContainerProps(configRoot:true)]
class SP_ChainedTask : SP_Task
{
	//------------------------------------------------------------------------------------------------------------//
	[Attribute()]
	ref array <ref SP_Task> m_aTasklist;
	//------------------------------------------------------------------------------------------------------------//
	[Attribute()]
	bool AssignOnInit;
	//------------------------------------------------------------------------------------------------------------//
	[Attribute()]
	FactionKey key;
	//------------------------------------------------------------------------------------------------------------//
	ref array <ref SP_Task> m_aTasks;
	//------------------------------------------------------------------------------------------------------------//
	private int stage;
	//------------------------------------------------------------------------------------------------------------//
	override IEntity GetOwner(){return  GetCurrentTask().GetOwner();};
	//------------------------------------------------------------------------------------------------------------//
	override IEntity GetTarget(){return  GetCurrentTask().GetTarget();};
	//------------------------------------------------------------------------------------------------------------//
	override bool CharacterIsOwner(IEntity Character){return  GetCurrentTask().CharacterIsOwner(Character);};
	//------------------------------------------------------------------------------------------------------------//
	override bool CharacterIsTarget(IEntity Character){return  GetCurrentTask().CharacterIsTarget(Character);}
	//------------------------------------------------------------------------------------------------------------//
	override string GetCompletionText(IEntity Completionist){return GetCurrentTask().GetCompletionText(Completionist);};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_ChainedTask;}
	//------------------------------------------------------------------------------------------------------------//
	override string GetTaskDescription(){return m_aTasks[stage].m_sTaskDesc;}
	//------------------------------------------------------------------------------------------------------------//
	override string GetTaskDiag(){return m_aTasks[stage].m_sTaskDiag;}
	//------------------------------------------------------------------------------------------------------------//
	override ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		CharacterHolder CharHolder = m_RequestManager.GetCharacterHolder();
		ChimeraCharacter Char;
		if (CharHolder)
			CharHolder.GetRandomUnit(Char);
		if (Char)
			Owner = Char;
		if(Owner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetTasklist(out array <ref SP_Task> OutTasklist)
	{
		foreach (SP_Task task : m_aTasklist)
		{
			OutTasklist.Insert(task);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void SetTasklist(array <ref SP_Task> OutTasklist)
	{
		if (!m_aTasklist)
			m_aTasklist = new array <ref SP_Task>();
		foreach (SP_Task task : m_aTasklist)
		{
			m_aTasklist.Insert(task);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	bool InitCurrentStage()
	{
		if (!m_aTasks[stage].Init())
			return false;
		m_aTasks[stage].OnTaskFinished().Insert(Progress);
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	void Progress(SP_Task task)
	{
		if (task.e_State == ETaskState.COMPLETED)
		{
			task.OnTaskFinished().Remove(Progress);
			if (stage + 1 == m_aTasks.Count())
			{
				CompleteChainedTask(m_aTaskAssigned[0]);
			}
			else
			{
				stage += 1;
				if (!InitCurrentStage())
					FailTask(EDamageState.DESTROYED);
				GetCurrentTask().AssignCharacter(m_aTaskAssigned[0]);
			}
		}
		else if (task.e_State == ETaskState.FAILED)
		{
			task.OnTaskFinished().Remove(Progress);
			FailTask(EDamageState.DESTROYED);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	override void AssignCharacter(IEntity Character)
	{
		if (!m_aTaskAssigned.Contains(Character))
			m_aTaskAssigned.Insert(Character);
		if(m_aTaskAssigned.Count() > 0 && e_State == ETaskState.UNASSIGNED)
		{
			e_State = ETaskState.ASSIGNED;
		}
		m_aTasks[stage].AssignCharacter(Character);
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool CharacterAssigned(IEntity Character)
	{
		if(m_aTasks[stage].m_aTaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		SP_Task task = m_aTasks[stage];
		return task.ReadyToDeliver(TalkingChar, Assignee);
	};
	//------------------------------------------------------------------------------------------------------------//
	SP_Task GetCurrentTask()
	{
		if (!m_aTasks[stage])
			return null;
		return m_aTasks[stage];
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool CompleteTask(IEntity Assignee){return m_aTasks[stage].CompleteTask(Assignee);};
	//------------------------------------------------------------------------------------------------------------//
	override void FailTask(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(GetCurrentTask().GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		e_State = ETaskState.FAILED;
		GetOnTaskFinished(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	void CompleteChainedTask(IEntity Assignee)
	{
		e_State = ETaskState.COMPLETED;
		m_eCopletionist = Assignee;
		GetOnTaskFinished(this);
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool Init()
	{
		m_RequestManager = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if (!m_aTasklist)
		{
			m_aTasklist = new array <ref SP_Task>();
			SP_ChainedTask tasksample = SP_ChainedTask.Cast(m_RequestManager.GetTaskSample(SP_ChainedTask));
			if(!tasksample)
			{
				return false;
			}
			tasksample.GetTasklist(m_aTasklist);
		}
		foreach (SP_Task sample : m_aTasklist)
		{
			if (!m_aTasks)
				m_aTasks = new array <ref SP_Task>();
			SP_Task task = SP_Task.Cast(sample.GetClassName().Spawn());
			m_aTasks.Insert(task);
			task.m_sTaskOwnerOverride = sample.m_sTaskOwnerOverride;
			task.m_sTaskTargetOverride = sample.m_sTaskTargetOverride;
			task.m_iRewardAverageAmount = sample.m_iRewardAverageAmount;
			
		}
		if (!InitCurrentStage())
			return false;
		//-------------------------------------------------//
		e_State = ETaskState.UNASSIGNED;
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(GetCurrentTask().GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(FailTask);
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
}