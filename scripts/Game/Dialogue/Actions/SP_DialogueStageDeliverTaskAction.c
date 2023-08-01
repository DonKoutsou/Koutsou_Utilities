[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageDeliverTaskAction : DialogueStage
{
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if(requestman.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			requestman.GetCharTasks(Character, MyTasks);
			for (int i, count = MyTasks.Count(); i < count; i++)
			{
				if(MyTasks[i].ReadyToDeliver(Character, Player))
				{
					Diag.Escape(Character, Player);
					MyTasks[i].CompleteTask(Player);
				}
			}
		}
		if(requestman.CharIsTarget(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			requestman.GetCharTargetTasks(Character, MyTasks);
			for (int i, count = MyTasks.Count(); i < count; i++)
			{
				if(MyTasks[i].ReadyToDeliver(Character, Player))
				{
					Diag.Escape(Character, Player);
					MyTasks[i].CompleteTask(Player);
				}
			}
		}
	};
	override bool CanBeShown(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if(requestman.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			requestman.GetCharTasks(Character, MyTasks);
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
		if(requestman.CharIsTarget(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			requestman.GetCharTargetTasks(Character, MyTasks);
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
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		int delivertaskam;
		int bountytaskam;
		int Requesttaskam;
		int Navigatetaskam;
		if(requestman.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			requestman.GetCharTasks(Character, MyTasks);
			foreach (SP_Task Task : MyTasks)
			{
				if(Task.ReadyToDeliver(Character, Player))
				{
					SP_ChainedTask Chained = SP_ChainedTask.Cast(Task);
					if (Chained)
						Task = Chained.GetCurrentTask();
					SP_BountyTask BountyT = SP_BountyTask.Cast(Task);
					if(BountyT)
					{
						bountytaskam += 1;
					}
					SP_RetrieveTask RetrieveT = SP_RetrieveTask.Cast(Task);
					if(RetrieveT)
					{
						Requesttaskam += 1;
					}
					SP_NavigateTask NavigateT = SP_NavigateTask.Cast(Task);
					if(NavigateT)
					{
						Navigatetaskam += 1;
					}
					
				}
			}
		}
		if(requestman.CharIsTarget(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			requestman.GetCharTargetTasks(Character, MyTasks);
			foreach (SP_Task Task : MyTasks)
			{
				if(Task.ReadyToDeliver(Character, Player))
				{
					SP_ChainedTask Chained = SP_ChainedTask.Cast(Task);
					if (Chained)
						Task = Chained.GetCurrentTask();
					SP_DeliverTask DelT = SP_DeliverTask.Cast(Task);
					if(DelT)
					{
						delivertaskam += 1;
					}
				}
			}
		}
		if (delivertaskam > 0)
		{
	    if (bountytaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0)
				{
	        acttext = "I have a delivery for you.";
	        return true;
	    	}
	    else if (bountytaskam > 0 || Requesttaskam > 0 ||  Navigatetaskam > 0)
				{
	        acttext = "I have some tasks to deliver.";
	        return true;
	    	}
		}
		if (bountytaskam > 0) 
			{
				if (delivertaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0) 
					{
				  	acttext = "I've completed the bounty.";
				    return true;
				  }
				else if (delivertaskam > 0 || Requesttaskam > 0 || Navigatetaskam > 0)
					{
			    	acttext = "I have some tasks to deliver.";
			      return true;
			    }
			}
		if (Requesttaskam > 0)
			{
		    if (delivertaskam == 0 && bountytaskam == 0 && Navigatetaskam == 0)
				{
		    	acttext = "I've brought the items you asked.";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Navigatetaskam > 0)
				{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
			}
		if (Navigatetaskam > 0)
			{
		    if (delivertaskam == 0 && bountytaskam == 0 && Requesttaskam == 0)
				{
		    	acttext = "I've brought you where you asked.";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Requesttaskam > 0)
				{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
			}
		return false;
	}

};