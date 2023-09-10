class SCR_AIPerformLightAction : AITaskScripted
{
	protected const static string ENTITY_PORT = "TargetEntity";
	protected const static string TAG_PORT = "Tags";
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		ENTITY_PORT, TAG_PORT
	};
	
	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		if(owner)
		{
			IEntity targetEntity;
			string userActionString;
			typename userAction;
			GetVariableIn(ENTITY_PORT, targetEntity);
			array <string> tags = {};
			GetVariableIn(TAG_PORT, tags);
			if (tags.IsEmpty())
				return ENodeResult.FAIL;
			
			if (!targetEntity)
				return ENodeResult.FAIL;
			if(tags[0] == "LightFire")
			{
				userActionString = "SCR_LightFireplaceUserAction";
			}
			else if(tags[0] == "SwitchLight")
			{
				userActionString = "SCR_SwitchLightUserAction";
			}
			else if(tags[0] == "SwitchRadio")
			{
				userActionString = "SCR_TurnOnAction";
			}
			else if(tags[0] == "DeadBody")
			{
				userActionString = "SP_AILootBodyAction";
			}
			else if(tags[0] == "SmartTask")
			{
				userActionString = "SP_DialogueAction";
			}
			else if (tags[0] == "StorePost")
			{
				userActionString = "SP_BuyStuff";
			}
			IEntity controlledEntity = owner.GetControlledEntity();
			if (!controlledEntity)
				return ENodeResult.FAIL;
	
			
	
			userAction = userActionString.ToType();
			if (!userAction)
				return ENodeResult.FAIL;
	
			array<BaseUserAction> outActions = {};
			ScriptedUserAction action;
			GetActions(targetEntity, outActions);
			foreach (BaseUserAction baseAction : outActions)
			{
				action = ScriptedUserAction.Cast(baseAction);
				if (action && userAction == action.Type() && action.CanBePerformedScript(controlledEntity))
				{
					action.PerformAction(targetEntity, controlledEntity);
					return ENodeResult.SUCCESS;
				}
			}
		}
		return ENodeResult.FAIL;			
	}
	
	//------------------------------------------------------------------------------------------------
	private void GetActions(IEntity targetEntity, notnull out array<BaseUserAction> outActions)
	{
		if (!targetEntity)
			return;
		
		ActionsManagerComponent actionOnEntity = ActionsManagerComponent.Cast(targetEntity.FindComponent(ActionsManagerComponent));
		
		if (!actionOnEntity)
			return;
		
		actionOnEntity.GetActionsList(outActions);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
};
class SCR_AIPerformTaskAction : AITaskScripted
{
	protected static string TARGETENTITY_IN_PORT = "TargetEntity";
	
	protected static string TASK_IN_PORT = "Task";
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = 
	{
		TARGETENTITY_IN_PORT,
		TASK_IN_PORT
	};
	
	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		if(owner)
		{
			IEntity targetEntity;
			GetVariableIn(TARGETENTITY_IN_PORT, targetEntity);
			ref SP_Task task;
			GetVariableIn(TASK_IN_PORT, task);
			if (!targetEntity)
				return ENodeResult.FAIL;
			string userActionString = "SP_DialogueAction";
			
			IEntity controlledEntity = owner.GetControlledEntity();
			if (!controlledEntity)
				return ENodeResult.FAIL;
			
			typename userAction;
			userAction = userActionString.ToType();
			if (!userAction)
				return ENodeResult.FAIL;
	
			array<BaseUserAction> outActions = {};
			SP_DialogueAction action;
			GetActions(targetEntity, outActions);
			foreach (BaseUserAction baseAction : outActions)
			{
				action = SP_DialogueAction.Cast(baseAction);
				if (action && userAction == action.Type() && action.CanBePerformedScript(controlledEntity))
				{
					if (task)
					{
						action.taskstogive.Insert(controlledEntity ,task);
					}
					action.PerformAction(targetEntity, controlledEntity);
					return ENodeResult.SUCCESS;
				}
			}
		}
		return ENodeResult.FAIL;			
	}
	
	//------------------------------------------------------------------------------------------------
	private void GetActions(IEntity targetEntity, notnull out array<BaseUserAction> outActions)
	{
		if (!targetEntity)
			return;
		
		ActionsManagerComponent actionOnEntity = ActionsManagerComponent.Cast(targetEntity.FindComponent(ActionsManagerComponent));
		
		if (!actionOnEntity)
			return;
		
		actionOnEntity.GetActionsList(outActions);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
};
class SCR_AIPerformCompleteTaskAction : AITaskScripted
{
	protected static string TARGETENTITY_IN_PORT = "TargetEntity";
	
	protected static string TASK_IN_PORT = "Task";
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = 
	{
		TARGETENTITY_IN_PORT,
		TASK_IN_PORT
	};
	
	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		if(owner)
		{
			IEntity targetEntity;
			GetVariableIn(TARGETENTITY_IN_PORT, targetEntity);
			ref SP_Task task;
			GetVariableIn(TASK_IN_PORT, task);
			if (!targetEntity)
				return ENodeResult.FAIL;
			string userActionString = "SP_DialogueAction";
			
			IEntity controlledEntity = owner.GetControlledEntity();
			if (!controlledEntity)
				return ENodeResult.FAIL;
			
			typename userAction;
			userAction = userActionString.ToType();
			if (!userAction)
				return ENodeResult.FAIL;
	
			array<BaseUserAction> outActions = {};
			SP_DialogueAction action;
			GetActions(targetEntity, outActions);
			foreach (BaseUserAction baseAction : outActions)
			{
				action = SP_DialogueAction.Cast(baseAction);
				if (action && userAction == action.Type() && action.CanBePerformedScript(controlledEntity))
				{
					///if (task)
					//{
					//	action.task = task;
					//}
					action.PerformAction(targetEntity, controlledEntity);
					if (task.GetState() == ETaskState.COMPLETED)
						return ENodeResult.SUCCESS;
				}
			}
		}
		return ENodeResult.FAIL;			
	}
	
	//------------------------------------------------------------------------------------------------
	private void GetActions(IEntity targetEntity, notnull out array<BaseUserAction> outActions)
	{
		if (!targetEntity)
			return;
		
		ActionsManagerComponent actionOnEntity = ActionsManagerComponent.Cast(targetEntity.FindComponent(ActionsManagerComponent));
		
		if (!actionOnEntity)
			return;
		
		actionOnEntity.GetActionsList(outActions);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
};