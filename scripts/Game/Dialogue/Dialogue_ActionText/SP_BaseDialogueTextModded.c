[BaseContainerProps(configRoot:true)]
class SP_LocationText : DS_BaseDialogueText
{
	[Attribute(desc: "This text will be added before the current location of the player/NPC")]
	string m_sTextToAddToLocation;
	
	[Attribute(desc: "if set to false intsead of player, NPC location will be used", defvalue: "1")]
	bool m_bTestPlayer;
	
	override string GetText(IEntity Character, IEntity Player)
	{
		if (m_bTestPlayer)
			return m_sTextToAddToLocation + " " + DS_DialogueComponent.GetCharacterLocation(Player, true);
		else
			return m_sTextToAddToLocation + " " + DS_DialogueComponent.GetCharacterLocation(Character, true);
	};
}
[BaseContainerProps(configRoot:true)]
class SP_RummorText : DS_BaseDialogueText
{
}
[BaseContainerProps(configRoot:true)]
class SP_TaskEntryText : DS_BaseDialogueText
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
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskInfoType))]
	int m_iTaskTextToShow;
	override string GetText(IEntity Character, IEntity Player)
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
		GatherTasks(CharToCheck, CharToAssign, tasklist);
		if (m_iTaskTextToShow == ETaskInfoType.TaskState)
			m_sActionText = typename.EnumToString(ETaskState, tasklist[m_iIndex].GetState());
		if (m_iTaskTextToShow == ETaskInfoType.TaskDialogue)
			m_sActionText = tasklist[m_iIndex].GetTaskDiag();
		if (m_iTaskTextToShow == ETaskInfoType.TaskTitle)
			m_sActionText = tasklist[m_iIndex].GetTaskTitle();
		if (m_iTaskTextToShow == ETaskInfoType.ActionText)
			m_sActionText = tasklist[m_iIndex].GetActionText();
		if (m_iTaskTextToShow == ETaskInfoType.AcceptText)
			m_sActionText = tasklist[m_iIndex].GetAcceptText();		
		if (m_iTaskTextToShow == ETaskInfoType.CompletionText)
			m_sActionText = tasklist[m_iIndex].GetCompletionText(Player);
		return super.GetText(Character, Player);
	}
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
	}
}
[BaseContainerProps(configRoot:true)]
class SP_AvailableTasksEntryText : SP_TaskEntryText
{
	override string GetText(IEntity Character, IEntity Player)
	{
		array <ref SP_Task> tasklist = {};
		GatherTasks(Character, Player, tasklist);
		m_sActionText = "Here is what i have. \n ";
		foreach (SP_Task task : tasklist)
		{
			m_sActionText = m_sActionText + task.GetTaskDiag() + "\n";
		};
		return super.GetText(Character, Player);
	}
	
}
enum ETaskInfoType
{
	TaskState,
	TaskDialogue,
	TaskTitle,
	ActionText,
	AcceptText,
	CompletionText
};