[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class SP_DialogueStageDeliverTaskAction : DS_BaseDialogueStageAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		DS_DialogueComponent Diag = DS_DialogueComponent.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if(SP_RequestManagerComponent.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharOwnedTasks(Character, MyTasks);
			for (int i, count = MyTasks.Count(); i < count; i++)
			{
				if(MyTasks[i].ReadyToDeliver(Character, Player))
				{
					//Diag.Escape(Character, Player);
					//OwnerStage.m_sDialogueText.SetText(MyTasks[i].GetCompletionText(Player));
					MyTasks[i].CompleteTask(Player);
					SP_ChainedTask Chained = SP_ChainedTask.Cast(MyTasks[i]);
					//if (Chained && Chained.GetState() != ETaskState.COMPLETED)
						//OwnerStage.m_sDialogueText.SetText(OwnerStage.m_sDialogueText.GetText(Character, Player) + " " + Chained.GetTaskDiag());
				}
			}
		}
		if(SP_RequestManagerComponent.CharIsTarget(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharTargetTasks(Character, MyTasks);
			for (int i, count = MyTasks.Count(); i < count; i++)
			{
				if(MyTasks[i].ReadyToDeliver(Character, Player))
				{
					//Diag.Escape(Character, Player);
					//OwnerStage.m_sDialogueText.SetText(MyTasks[i].GetCompletionText(Player));
					MyTasks[i].CompleteTask(Player);
					SP_ChainedTask Chained = SP_ChainedTask.Cast(MyTasks[i]);
					//if (Chained && Chained.GetState() != ETaskState.COMPLETED)
						//OwnerStage.m_sDialogueText.SetText(OwnerStage.m_sDialogueText.GetText(Character, Player) + " " + Chained.GetTaskDiag());
				}
			}
		}
		super.Perform(Character, Player);
	};
	
	

};
