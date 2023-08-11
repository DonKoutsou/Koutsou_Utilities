class SCR_AIGetSmartActionParamsActionOffset : AITaskScripted
{
	static const string SMART_ACTION_GUARD_PORT		= "SmartActionGuard";
	static const string POSITION_PORT				= "OutPos";
	static const string	ACTION_OFFSET_PORT				= "ActionOffset";
		
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		SMART_ACTION_GUARD_PORT
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		POSITION_PORT,
		ACTION_OFFSET_PORT
	};
	override TStringArray GetVariablesOut()
    {
        return s_aVarsOut;
    }

	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AISmartActionComponent smartAction;
		if(!GetVariableIn(SMART_ACTION_GUARD_PORT, smartAction) || !smartAction)
		{
			NodeError(this, owner, "Invalid SCR_AISmartActionSentinelComponent provided!");
			return ENodeResult.FAIL;
		}
		
		vector worldPosition;
		vector mat[4];
		worldPosition = smartAction.GetOwner().GetOrigin();
		SetVariableOut(ACTION_OFFSET_PORT, worldPosition + smartAction.GetActionOffset());
		SetVariableOut(POSITION_PORT, worldPosition);
		return ENodeResult.SUCCESS;
	}	
};
