class SCR_AIPerformLightAction : AITaskScripted
{
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		"TargetEntity"
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
			GetVariableIn("TargetEntity", targetEntity);
			if (!targetEntity)
				return ENodeResult.FAIL;
			if(SCR_FireplaceComponent.Cast(targetEntity.FindComponent(SCR_FireplaceComponent)))
			{
				userActionString = "SCR_LightFireplaceUserAction";
			}
			else if(SCR_BaseInteractiveLightComponent.Cast(targetEntity.FindComponent(SCR_BaseInteractiveLightComponent)))
			{
				userActionString = "SCR_SwitchLightUserAction";
			}
			else if(RadioBroadcastComponent.Cast(targetEntity.FindComponent(RadioBroadcastComponent)))
			{
				userActionString = "SCR_TurnOnAction";
			}
			else if(ChimeraCharacter.Cast(targetEntity))
			{
				userActionString = "SP_AIStab";
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
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		"TargetEntity"
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
			string userActionString = "SP_DialogueAction";
			typename userAction;
			GetVariableIn("TargetEntity", targetEntity);
			if (!targetEntity)
				return ENodeResult.FAIL;
	
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