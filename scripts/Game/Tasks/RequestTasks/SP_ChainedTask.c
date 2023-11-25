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
	override string GetTaskDescription(){return GetCurrentTask().GetTaskDescription();}
	//------------------------------------------------------------------------------------------------------------//
	override string GetTaskDiag(){return GetCurrentTask().GetTaskDiag();}
	//------------------------------------------------------------------------------------------------------------//
	override string GetActionText(){return GetCurrentTask().GetActionText();}
	//------------------------------------------------------------------------------------------------------------//
	override ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (!CharacterHolder.GetRandomUnit(Char))
			return false;
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
		m_aTasks[stage].SetPartOfChain();
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
				CompleteChainedTask(m_aTaskAssigned);
			}
			else
			{
				stage += 1;
				while (!InitCurrentStage())
					SkipStage(task);
				GetCurrentTask().AssignCharacter(m_aTaskAssigned);
			}
		}
		else if (task.e_State == ETaskState.FAILED)
		{
			task.OnTaskFinished().Remove(Progress);
			FailTask();
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void SkipStage(SP_Task task)
	{
		if (stage + 1 == m_aTasks.Count())
		{
			CompleteChainedTask(m_aTaskAssigned);
		}
		else
		{
			stage += 1;
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignCharacter(IEntity Character)
	{
		if (!m_aTaskAssigned)
			m_aTaskAssigned = Character;
		if(m_aTaskAssigned && e_State == ETaskState.UNASSIGNED)
		{
			e_State = ETaskState.ASSIGNED;
		}
		m_aTasks[stage].AssignCharacter(Character);
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool CharacterAssigned(IEntity Character)
	{
		if(m_aTasks[stage].m_aTaskAssigned == Character)
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
	override bool TimeLimitHasPassed()
	{
		SP_Task task = m_aTasks[stage];
		return task.TimeLimitHasPassed();
	}
	//------------------------------------------------------------------------------------------------------------//
	override void SetTimeLimit()
	{
		SP_Task task = m_aTasks[stage];
		task.SetTimeLimit();
	}
	//------------------------------------------------------------------------------------------------------------//
	override float GetTimeLimit()
	{
		SP_Task task = m_aTasks[stage];
		return task.GetTimeLimit();
	}
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
	override void FailTask()
	{
		DS_DialogueComponent Diag = DS_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(DS_DialogueComponent));
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
		if (!m_aTasklist)
		{
			m_aTasklist = new array <ref SP_Task>();
			SP_ChainedTask tasksample = SP_ChainedTask.Cast(SP_RequestManagerComponent.GetTaskSample(SP_ChainedTask));
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
			//SP_Task task = SP_Task.Cast(sample.GetClassName().Spawn());
			m_aTasks.Insert(sample);
			//task.m_sTaskOwnerOverride = sample.m_sTaskOwnerOverride;
			//task.m_sTaskTargetOverride = sample.m_sTaskTargetOverride;
			//task.m_iRewardAverageAmount = sample.m_iRewardAverageAmount;
			//task.e_RewardLabel = sample.e_RewardLabel;
		}
		if (!InitCurrentStage())
			return false;
		//-------------------------------------------------//
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
}