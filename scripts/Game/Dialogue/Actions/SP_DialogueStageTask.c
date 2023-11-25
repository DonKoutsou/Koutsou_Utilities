[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class SP_DialogueStageTaskAction : DS_BaseDialogueStageAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent TaskMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		SP_Task t_Task;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		TaskMan.GetCharOwnedTasks(Character, tasks);
		if (tasks.Count() >= m_iIndex)
		{
			return;
		}
		t_Task = tasks.Get(m_iIndex);
		t_Task.AssignCharacter(Player);
		super.Perform(Character, Player);
	};
}