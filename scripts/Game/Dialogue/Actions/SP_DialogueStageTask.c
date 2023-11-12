[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTask : DialogueStage
{
	SP_Task t_Task;
	override void Perform(IEntity Character, IEntity Player)
	{
		t_Task.AssignCharacter(Player);
		super.Perform(Character, Player);
	};
	void SetupTask(SP_Task task)
	{
		t_Task = task;
	}
	override bool GetActionText(IEntity Character, IEntity Player, out string acttext)
	{
		if(t_Task && !t_Task.CharacterAssigned(Player) && t_Task.GetState() != ETaskState.ASSIGNED)
		{
			acttext = t_Task.GetAcceptText();
			return true;
		}
		return false;
	}
	override string GetStageDialogueText(IEntity Character, IEntity Player)
		{
			return "Thanks alot. Good luck!";
		}
}