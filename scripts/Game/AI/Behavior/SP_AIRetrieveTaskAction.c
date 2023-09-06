class SCR_AIExecuteRetrieveTaskBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<SP_Task> m_vTask = new SCR_BTParam<SP_Task>("Task");
	
	SP_Task PickedTask;
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	
	bool m_bActiveFollowing = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(SP_Task Task)
	{
		m_vTask.Init(this, Task);
	}
	
	// posWorld - position to observe
	void SCR_AIExecuteRetrieveTaskBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, SP_Task Task)
	{
		InitParameters(Task);
		PickedTask = Task;	
		m_sBehaviorTree = "{6A730A33C247E1E3}AI/BehaviorTrees/SP_AITaskAction-Retrieve.bt";
		m_bAllowLook = true; // Disable standard looking
		m_bResetLook = true;
		m_bActiveFollowing = true;
	}
	
	void SetActiveFollowing(bool state)
	{
		m_bActiveFollowing = state;
	}
	
	override float EvaluatePriorityLevel()
	{
		// Fail action if timeout has been reached
		//float currentTime_ms = GetGame().GetWorld().GetWorldTime();
		//if (currentTime_ms > m_fDeleteActionTime_ms)
		//{
		//	Fail();
		//	return 0;
		//}
		//return m_fPriority;
		if (m_bActiveFollowing)
			return 100;
		
		Fail();
		return 0;
	}
	
	void InitTimeout(float timeout_s)
	{
		float currentTime_ms = GetGame().GetWorld().GetWorldTime(); // Milliseconds!
		m_fDeleteActionTime_ms = currentTime_ms + 10 * timeout_s;
	}
};
class SCR_AIRetrieveTaskParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIExecuteRetrieveTaskBehavior(null, null, null)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};