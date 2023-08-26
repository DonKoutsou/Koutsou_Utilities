class SCR_AIExecuteNavigateTaskBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<SP_Task> m_vTask = new SCR_BTParam<SP_Task>("Task");
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	
	bool m_bActiveFollowing = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(SP_Task Task)
	{
		m_vTask.Init(this, Task);
	}
	
	// posWorld - position to observe
	void SCR_AIExecuteNavigateTaskBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, SP_Task Task)
	{
		InitParameters(Task);
				
		m_sBehaviorTree = "{A670B53B3B00B2D2}AI/BehaviorTrees/SP_AITaskAction-Navigate.bt";
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
class SCR_AINavigateTaskParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIExecuteNavigateTaskBehavior(null, null, null)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};