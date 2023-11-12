[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageDeliverTaskAction : DialogueStage
{
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if(SP_RequestManagerComponent.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharOwnedTasks(Character, MyTasks);
			for (int i, count = MyTasks.Count(); i < count; i++)
			{
				if(MyTasks[i].ReadyToDeliver(Character, Player))
				{
					//Diag.Escape(Character, Player);
					DialogueText = MyTasks[i].GetCompletionText(Player);
					MyTasks[i].CompleteTask(Player);
					SP_ChainedTask Chained = SP_ChainedTask.Cast(MyTasks[i]);
					if (Chained && Chained.GetState() != ETaskState.COMPLETED)
						DialogueText = DialogueText + " " + Chained.GetTaskDiag();
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
					DialogueText = MyTasks[i].GetCompletionText(Player);
					MyTasks[i].CompleteTask(Player);
					SP_ChainedTask Chained = SP_ChainedTask.Cast(MyTasks[i]);
					if (Chained && Chained.GetState() != ETaskState.COMPLETED)
						DialogueText = DialogueText + " " + Chained.GetTaskDiag();
				}
			}
		}
		super.Perform(Character, Player);
	};
	override bool CanBeShown(IEntity Character, IEntity Player)
	{
		if(SP_RequestManagerComponent.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharOwnedTasks(Character, MyTasks);
			foreach (SP_Task Task : MyTasks)
			{
				if(Task.ReadyToDeliver(Character, Player))
				{
					if(Task.GetState() != ETaskState.COMPLETED)
					{
						return true;
					}
				}
			}
		}
		if(SP_RequestManagerComponent.CharIsTarget(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharTargetTasks(Character, MyTasks);
			foreach (SP_Task Task : MyTasks)
			{
				if(Task.ReadyToDeliver(Character, Player))
				{
					if(Task.GetState() != ETaskState.COMPLETED)
					{
						return true;
					}
				}
			}
		}
		return false;
		
	}
	override bool GetActionText(IEntity Character, IEntity Player, out string acttext)
	{
		int taskam;
		array<ref SP_Task> MyReadyTasks = new array<ref SP_Task>();
		if(SP_RequestManagerComponent.CharHasTask(Character))
		{ 
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharOwnedTasks(Character, MyTasks);
			foreach (SP_Task Task : MyTasks)
			{
				if(Task.ReadyToDeliver(Character, Player))
				{
					MyReadyTasks.Insert(Task);
				}
			}
		}
		if(SP_RequestManagerComponent.CharIsTarget(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharTargetTasks(Character, MyTasks);
			foreach (SP_Task Task : MyTasks)
			{
				if(Task.ReadyToDeliver(Character, Player))
				{
					MyReadyTasks.Insert(Task);
				}
			}
		}
		if (MyReadyTasks.Count() > 1)
		{
			acttext = "I have some tasks to deliver.";
			return true;
		}
		if (MyReadyTasks.Count() == 1)
		{
			acttext = MyReadyTasks[0].GetActionText();
			return true;
		}
		return false;
	}

};