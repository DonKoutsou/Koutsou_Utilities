class SCR_AIExecuteDeliveryTaskBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<IEntity> m_vCharOwner = new SCR_BTParam<IEntity>("TaskOwner");
	protected ref SCR_BTParam<IEntity> m_vCharTarg = new SCR_BTParam<IEntity>("TaskTarget");
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	
	bool m_bActiveFollowing = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(IEntity TaskOwner, IEntity TaskTarget)
	{
		m_vCharOwner.Init(this, TaskOwner);
		m_vCharTarg.Init(this, TaskTarget);
	}
	
	// posWorld - position to observe
	void SCR_AIExecuteDeliveryTaskBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity TaskTarget, IEntity TaskOwner)
	{
		InitParameters(TaskOwner ,TaskTarget);
				
		m_sBehaviorTree = "{077A234FBE16E63A}AI/BehaviorTrees/SP_AITaskAction.bt";
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
class SCR_AIDeliveryTaskParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIExecuteDeliveryTaskBehavior(null, null, null, null)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};