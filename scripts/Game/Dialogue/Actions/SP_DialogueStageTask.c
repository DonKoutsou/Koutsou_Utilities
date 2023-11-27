[BaseContainerProps(configRoot:true)]
class SP_DialogueStageBaseTaskAction : DS_BaseDialogueStageAction
{
	[Attribute(defvalue: "1")]
	bool m_bGetOwnedTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetTargetTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetAssignedTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetReadyToDeliver;
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskType))]
	int m_iGetTasksOfType;
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskState))]
	int m_iGetTasksWithState;
	[Attribute(desc: "Show player tasks")]
	bool m_bUsePlayer;
	override void Perform(IEntity Character, IEntity Player)
	{
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageAssignTaskAction : SP_DialogueStageBaseTaskAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		}
		SP_RequestManagerComponent Reqman = SP_RequestManagerComponent.GetInstance();
		array <ref SP_Task> tasklist = {};
		if (m_bGetOwnedTasks)
			Reqman.GetCharOwnedTasks(CharToCheck, tasklist);
		if (m_bGetTargetTasks)
			Reqman.GetCharTargetTasks(CharToCheck, tasklist);
		if (m_bGetAssignedTasks)
			Reqman.GetassignedTasks(CharToCheck, tasklist);
		if (m_bGetReadyToDeliver)
			Reqman.GetReadyToDeliver(CharToCheck, tasklist, CharToAssign);
		if (m_iGetTasksOfType)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetTaskType() != m_iGetTasksOfType)
					tasklist.Remove(i);
			}
		}
		if (m_iGetTasksWithState)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetState() != m_iGetTasksWithState)
					tasklist.Remove(i);
			}
		}
		if (m_iIndex >= tasklist.Count())
		{
			return;
		}
		SP_Task t_Task = tasklist.Get(m_iIndex);
		t_Task.AssignCharacter(CharToAssign);
		super.Perform(Character, Player);
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageCompleteTaskAction : SP_DialogueStageBaseTaskAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		};
		SP_RequestManagerComponent Reqman = SP_RequestManagerComponent.GetInstance();
		array <ref SP_Task> tasklist = {};
		if (m_bGetOwnedTasks)
			Reqman.GetCharOwnedTasks(CharToCheck, tasklist);
		if (m_bGetTargetTasks)
			Reqman.GetCharTargetTasks(CharToCheck, tasklist);
		if (m_bGetAssignedTasks)
			Reqman.GetassignedTasks(CharToCheck, tasklist);
		if (m_bGetReadyToDeliver)
			Reqman.GetReadyToDeliver(CharToCheck, tasklist, CharToAssign);
		if (m_iGetTasksOfType)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetTaskType() != m_iGetTasksOfType)
					tasklist.Remove(i);
			}
		}
		if (m_iGetTasksWithState)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetState() != m_iGetTasksWithState)
					tasklist.Remove(i);
			}
		}
		if (m_iIndex >= tasklist.Count())
		{
			return;
		}
		SP_Task t_Task = tasklist.Get(m_iIndex);
		t_Task.CompleteTask(CharToAssign);
		super.Perform(Character, Player);
	};
};