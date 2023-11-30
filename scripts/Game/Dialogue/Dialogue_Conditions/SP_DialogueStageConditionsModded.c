[BaseContainerProps(configRoot:true)]
class SP_DialogueStageBaseTaskActionCondition : DS_BaseDialogueStageActionCondition
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
	
	override bool CanBePerformed(IEntity Character, IEntity Player){};
	void GatherTasks(IEntity Character, IEntity Player, array <ref SP_Task> tasklist)
	{
		SP_RequestManagerComponent Reqman = SP_RequestManagerComponent.GetInstance();
		
		if (m_bGetOwnedTasks)
			Reqman.GetCharOwnedTasks(Character, tasklist);
		if (m_bGetTargetTasks)
			Reqman.GetCharTargetTasks(Character, tasklist);
		if (m_bGetAssignedTasks)
			Reqman.GetassignedTasks(Character, tasklist);
		if (m_bGetReadyToDeliver)
			Reqman.GetReadyToDeliver(Character, tasklist, Player);
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
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageAvailableTaskActionCondition : SP_DialogueStageBaseTaskActionCondition
{
	[Attribute()]
	bool m_bUseIndex;
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		}
		array <ref SP_Task> tasklist = {};
		GatherTasks(CharToCheck, CharToAssign, tasklist);
		if (m_bUseIndex)
		{
			if (m_iIndex >= tasklist.Count())
				return false;
		}
		else
			if (tasklist.IsEmpty())
				return false;
		return true;
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageCanTaskBeAssignedActionCondition : SP_DialogueStageBaseTaskActionCondition
{
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		}
		array <ref SP_Task> tasklist = {};
		
		GatherTasks(CharToCheck, CharToAssign, tasklist);
		
		if (m_iIndex >= tasklist.Count())
			return false;
		if (tasklist[m_iIndex].CanBeAssigned(CharToCheck, CharToAssign))
			return true;
		
		return false;
	};
}
