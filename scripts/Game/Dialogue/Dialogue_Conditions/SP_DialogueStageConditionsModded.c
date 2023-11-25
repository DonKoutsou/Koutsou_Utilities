[BaseContainerProps(configRoot:true)]
class SP_DialogueStageDeliverTaskActionCondition : DS_BaseDialogueStageActionCondition
{
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		if(SP_RequestManagerComponent.GetInstance().CharHasTask(Character))
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
	/*override bool GetActionText(IEntity Character, IEntity Player, out string acttext)
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
	}*/
}