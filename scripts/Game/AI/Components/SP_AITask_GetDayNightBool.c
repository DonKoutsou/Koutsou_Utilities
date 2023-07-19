class SP_AIGetDNightBool : AITaskScripted
{
	protected static string ACTION_IN_PORT = "SMARTACTION";
	
	protected static string DnNBOOL_OUT_PORT = "DNIGHTBOOL";

	
	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AISmartActionComponent SmartAction;
		GetVariableIn(ACTION_IN_PORT, SmartAction);
		bool Dnightbool;
		if (SmartAction)
		{
			Dnightbool = SmartAction.GetDNightBool();
		}
		
		SetVariableOut(DnNBOOL_OUT_PORT, Dnightbool);
		
		return ENodeResult.SUCCESS;
	}
	protected static ref TStringArray s_aVarsOut = {DnNBOOL_OUT_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {ACTION_IN_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	//------------------------------------------------------------------------------------------------
	override bool VisibleInPalette() {return true;}
	
	//------------------------------------------------------------------------------------------------
	protected override string GetOnHoverDescription() {return "Returns bool from smart action";}
}