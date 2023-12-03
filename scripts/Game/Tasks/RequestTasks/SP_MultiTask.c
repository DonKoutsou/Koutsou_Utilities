//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_MultiTask: SP_Task
{
	//------------------------------------------------------------------------------------------------------------//
	[Attribute()]
	ref array <ref SP_Task> m_aTasklist;
	//------------------------------------------------------------------------------------------------------------//
	ref array <ref SP_Task> m_aTasks;
	//------------------------------------------------------------------------------------------------------------//
	private int stage;
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_MultiTask;}
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
	bool InitTasks()
	{
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.GetInstance();
		for (int i = 0 , count = m_aTasks.Count() ; i < count; i++)
		{
			m_aTasks[i].SetPartOfChain();
			if (!m_aTasks[i].Init())
				return false;
			m_aTasks[i].OnTaskFinished().Insert(Progress);
			reqman.m_aTaskMap.Insert(m_aTasks[i]);
			//m_aTasks[i].AssignCharacter(m_aTaskAssigned);
		}
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	void Progress(SP_Task task)
	{
		if (task.e_State == ETaskState.COMPLETED)
		{
			task.OnTaskFinished().Remove(Progress);
			if (stage < m_aTasks.Count())
			{
				stage += 1;
			}
			if (stage == m_aTasks.Count())
			{
				CompleteTask( m_aTaskAssigned );
			}
		}
		else if (task.e_State == ETaskState.FAILED)
		{
			task.OnTaskFinished().Remove(Progress);
			FailTask();
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignCharacter(IEntity Character)
	{
		if (!super.AssignCharacter(Character))
			return false;
		for (int i = 0 , count = m_aTasks.Count() ; i < count; i++)
		{
			m_aTasks[i].AssignCharacter(Character);
		}
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
		if (stage + 1 == m_aTasks.Count())
			return true;
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override void FailTask()
	{
		DS_DialogueComponent Diag = DS_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(DS_DialogueComponent));
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		e_State = ETaskState.FAILED;
		GetOnTaskFinished(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string OLoc;
		array <string> taskDesc = {};
		GetInfo(OName, OLoc, taskDesc);
		m_sTaskDesc = string.Format("Complete the tasks %1 provided and then return to meet him on %2.\nTasks : \n", OName, OLoc);
		for (int i = 0, count = taskDesc.Count(); i  < count; i++)
		{
			m_sTaskDesc = string.Format("%1%2\n", m_sTaskDesc, taskDesc[i]);
		}
		m_sTaskDiag = string.Format("Please complete these tasks and then come talk to me.", OName, OLoc);
		m_sTaskTitle = string.Format("Complete %1's tasks.", OName);
		m_sTaskCompletiontext = "Thanks the help %1.";
		m_sacttext = "I've completed the tasks you asked.";
	};
	void GetInfo(out string OName, out string OLoc, out array <string> taskDesc)
	{
		SCR_GameModeCampaign GM = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		if (!GM)
			return;
		DS_DialogueComponent Diag = GM.GetDialogueComponent();

		for (int i = 0, count = m_aTasks.Count(); i  < count; i++)
		{
			if (!m_eTaskOwner)
				m_eTaskOwner = m_aTasks[i].GetOwner();
			IEntity Target = m_aTasks[i].GetTarget();
			string Tname = m_aTasks[i].GetTaskDescription();
			taskDesc.Insert(Tname);
		}
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = Diag.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool Init()
	{
		m_eTaskOwner = GetGame().FindEntity(m_sTaskOwnerOverride);
		m_eTaskTarget =GetGame().FindEntity(m_sTaskOwnerOverride);
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
		if (!InitTasks())
			return false;
		CreateDescritions();
		AddOwnerInvokers();
		//-------------------------------------------------//
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
}