[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageDeliverTaskAction : DialogueStage
{
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if(SP_RequestManagerComponent.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharTasks(Character, MyTasks);
			for (int i, count = MyTasks.Count(); i < count; i++)
			{
				if(MyTasks[i].ReadyToDeliver(Character, Player))
				{
					//Diag.Escape(Character, Player);
					DialogueText = MyTasks[i].GetCompletionText(Player);
					MyTasks[i].CompleteTask(Player);
					SP_ChainedTask Chained = SP_ChainedTask.Cast(MyTasks[i]);
					if (Chained)
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
					if (Chained)
						DialogueText = DialogueText + " " + Chained.GetTaskDiag();
				}
			}
		}
	};
	override bool CanBeShown(IEntity Character, IEntity Player)
	{
		if(SP_RequestManagerComponent.CharHasTask(Character))
		{
			array<ref SP_Task> MyTasks = new array<ref SP_Task>();
			SP_RequestManagerComponent.GetCharTasks(Character, MyTasks);
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
			SP_RequestManagerComponent.GetCharTasks(Character, MyTasks);
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
		
		/*if (delivertaskam > 0)
		{
	   		if (bountytaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0 && TalkTtaskam == 0 && KillTtaskam == 0 && DestTtaskam == 0)
			{
		        acttext = "I have a delivery for you.";
		        return true;
	    	}
	    	else if (bountytaskam > 0 || Requesttaskam > 0 ||  Navigatetaskam > 0 || TalkTtaskam > 0 || KillTtaskam > 0 || DestTtaskam > 0)
			{
		        acttext = "I have some tasks to deliver.";
		        return true;
	    	}
		}
		if (bountytaskam > 0) 
		{
			if (delivertaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0 && TalkTtaskam == 0 && KillTtaskam == 0 && DestTtaskam == 0) 
			{
			  	acttext = "I've completed the bounty.";
			    return true;
			 }
			else if (delivertaskam > 0 || Requesttaskam > 0 || Navigatetaskam > 0 || TalkTtaskam > 0 || KillTtaskam > 0 || DestTtaskam > 0)
			{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
		}
		if (Requesttaskam > 0)
		{
		    if (delivertaskam == 0 && bountytaskam == 0 && Navigatetaskam == 0 && TalkTtaskam == 0 && KillTtaskam == 0 && DestTtaskam == 0)
				{
		    	acttext = "I've brought the items you asked.";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Navigatetaskam > 0 || TalkTtaskam > 0 || KillTtaskam > 0 || DestTtaskam > 0)
			{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
		}
		if (Navigatetaskam > 0)
		{
		    if (delivertaskam == 0 && bountytaskam == 0 && Requesttaskam == 0 && TalkTtaskam == 0 && KillTtaskam == 0 && DestTtaskam == 0)
			{
		    	acttext = "I've brought you where you asked.";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Requesttaskam > 0 || TalkTtaskam > 0 || KillTtaskam > 0 || DestTtaskam > 0)
				{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
		}
		if (TalkTtaskam > 0)
		{
		    if (delivertaskam == 0 && bountytaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0 && KillTtaskam == 0 && DestTtaskam == 0)
			{
		    	acttext = "I'm here to meet you.";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Requesttaskam > 0 || Navigatetaskam > 0 || KillTtaskam > 0 || DestTtaskam > 0)
				{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
		}
		if (KillTtaskam > 0)
		{
		    if (delivertaskam == 0 && bountytaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0 && TalkTtaskam == 0 && DestTtaskam == 0)
			{
		    	acttext = "I killed who you asked.";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Requesttaskam > 0 || Navigatetaskam > 0 || TalkTtaskam > 0 || DestTtaskam > 0)
			{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
		}
		if (DestTtaskam > 0)
		{
		    if (delivertaskam == 0 && bountytaskam == 0 && Requesttaskam == 0 && Navigatetaskam == 0 && TalkTtaskam == 0 && KillTtaskam == 0)
			{
		    	acttext = "I destroyed the  .";
		    	return true;
		    }
		    else if (delivertaskam > 0 || bountytaskam > 0 || Requesttaskam > 0 || Navigatetaskam > 0 || TalkTtaskam > 0 || KillTtaskam > 0)
			{
		    	acttext = "I have some tasks to deliver.";
		    	return true;
		    }
		}*/
		return false;
	}

};