[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageMultiResponse : DialogueStage
{
		[Attribute(defvalue: "Dialogue Text", desc: "Dialogue Text", category: "Dialogue")]
    string DialogueText2;
		override string GetStageDialogueText(IEntity Character, IEntity Player)
		{
			SP_RequestManagerComponent TaskMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			
			array <ref SP_Task> tasks = new array <ref SP_Task>();
			TaskMan.GetCharOwnedTasks(Character, tasks);
			if(tasks.Count() > 0)
			{
				string tasktext;
				foreach (SP_Task task : tasks)
				{
					if ( task.IsReserved())
						continue;
					if( task.GetState() == ETaskState.ASSIGNED)
						continue;
					if ( ! task.CanBeAssigned(Character, Player) )
						continue;
					if (!tasktext)
						tasktext =  task.GetTaskDiag();
					else
						tasktext =  tasktext + " \n " + task.GetTaskDiag();
				}
				if (tasktext)
					tasktext = DialogueText + " \n " + tasktext;
				return tasktext;
				
			}
		 	return DialogueText2;
		}
		override bool CheckIfStageCanBranch(IEntity Character, IEntity Player)
		{
			SP_RequestManagerComponent taskman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(Character)
			{
				
				array <ref SP_Task> tasks = new array <ref SP_Task>();
				taskman.GetCharOwnedTasks(Character, tasks);
				if(tasks.Count() > 0)
				{
					foreach (SP_Task taskcheck : tasks)
					{
						SP_ChainedTask questlinetask = SP_ChainedTask.Cast(taskcheck);
						if (questlinetask)
							continue;
						if ( taskcheck.IsReserved())
							continue;
						if ( ! taskcheck .CanBeAssigned(Character, Player) )
							continue;
						if(taskcheck.GetState() == ETaskState.UNASSIGNED)
						{
							return true;
						}
						
					}
				}
			}
			return false;
		}
		override void Perform(IEntity Character, IEntity Player)
		{
			SP_RequestManagerComponent TaskMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			
			array<ref SP_Task> tasks = new array<ref SP_Task>();
			TaskMan.GetCharOwnedTasks(Character, tasks);
			for (int i, count = m_Branch.Count(); i < count; i++)
			{
				int taskcount = tasks.Count();
				if(taskcount >= i + 1)
				{
					DialogueStage stage;
					m_Branch[i].GetDialogueStage(Character, stage);
					DialogueStageTask StageTask = DialogueStageTask.Cast(stage);
					StageTask.SetupTask(tasks[i]);
				}
				else
				{
					DialogueStage stage;
					m_Branch[i].GetDialogueStage(Character, stage);
					DialogueStageTask StageTask = DialogueStageTask.Cast(stage);
					StageTask.SetupTask(null);
				}
			}
		};
}