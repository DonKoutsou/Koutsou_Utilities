class SCR_AIGetTaskParams : AITaskScripted
{
	static const string TASK_PORT = "Task";
	static const string TASK_OWNER_PORT		= "TaskOwner";
	static const string TASK_TARGET_PORT				= "TaskTarget";
		
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		TASK_PORT
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		TASK_OWNER_PORT,
		TASK_TARGET_PORT,
	};
	override TStringArray GetVariablesOut()
    {
        return s_aVarsOut;
    }

	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SP_Task Task;
		GetVariableIn(TASK_PORT, Task);
		if(!Task)
		{
			//NodeError(this, owner, "Invalid Task Provided!");
			return ENodeResult.FAIL;
		}
		SetVariableOut(TASK_OWNER_PORT, Task.GetOwner());
		SetVariableOut(TASK_TARGET_PORT, Task.GetTarget());
		return ENodeResult.SUCCESS;
	}	
};