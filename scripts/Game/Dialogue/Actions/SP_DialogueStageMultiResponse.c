[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageMultiResponse : DialogueStage
{
		[Attribute(defvalue: "Dialogue Text", desc: "Dialogue Text", category: "Dialogue")]
    string DialogueText2;
		override string GetStageDialogueText(IEntity Character, IEntity Player)
		{
			SP_RequestManagerComponent TaskMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			
			array <ref SP_Task> tasks = new array <ref SP_Task>();
			TaskMan.GetCharTasks(Character, tasks);
			if(tasks.Count() > 0)
			{
				foreach (SP_Task task : tasks)
				{
					if(task.CharacterAssigned(Player) == false)
					{
						return DialogueText;
					}
				}
				
			}
		 	return DialogueText2;
		}
		override bool CheckIfStageCanBranch(IEntity Character, IEntity Player)
		{
			SP_RequestManagerComponent taskman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(Character)
			{
				array <ref SP_Task> tasks = new array <ref SP_Task>();
				taskman.GetCharTasks(Character, tasks);
				if(tasks.Count() > 0)
				{
					foreach (SP_Task taskcheck : tasks)
					{
						if(taskcheck.CharacterAssigned(Player) == false)
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
			TaskMan.GetCharTasks(Character, tasks);
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