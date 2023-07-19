
[BaseContainerProps(configRoot:true),  DialogueBranchConfigTitleAttribute()]
class SP_DialogueTaskBranch : SP_DialogueBranch
{
	override bool CheckIfStageBranches(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent taskman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		return taskman.CharHasTask(Character);
	}
};
