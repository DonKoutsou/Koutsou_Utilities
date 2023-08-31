class SCR_AISetActionOccupied : AITaskScripted
{
	[Attribute ()]
	bool m_bShouldUnoccupyOnAbort;
	SCR_AISmartActionComponent myaction;
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		"TargetAction"
	};
	
	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	override event void OnAbort(AIAgent owner, Node nodeCausingAbort)
	{
		if (m_bShouldUnoccupyOnAbort && myaction)
		{
			myaction.SetActionAccessible(true);
		}
	}
	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn("TargetAction", myaction);
		if(myaction)
		{
			myaction.SetActionAccessible(false);
			return ENodeResult.SUCCESS;
		}
		return ENodeResult.FAIL;			
	}
	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
};
class SCR_AISetActionUnOccupied : AITaskScripted
{
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		"TargetAction"
	};
	
	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AISmartActionComponent myaction;
		GetVariableIn("TargetAction", myaction);
		if(myaction)
		{
		myaction.SetActionAccessible(true);
		return ENodeResult.SUCCESS;	
		}
		return ENodeResult.FAIL;		
	}
	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
};