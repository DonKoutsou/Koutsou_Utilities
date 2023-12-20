class SCR_AIFulfillServiceNeedBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam <IEntity> m_vEntity = new SCR_BTParam <IEntity>("Arsenal");
	
	//protected ref SCR_BTParam<bool> m_bUseBinoculars = new SCR_BTParam<bool>("UseBinoculars"); // Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	IEntity Owner;
	
	bool m_bActiveBuilding = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(IEntity Arsenal)
	{
		m_vEntity.Init(this, Arsenal);
		//m_bUniqueInActionQueue = false;
		//m_bUseBinoculars.Init(this, false);
	}
	
	// posWorld - position to observe
	void SCR_AIFulfillServiceNeedBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity Arsenal)
	{
		m_sBehaviorTree = "{D65DC09987CECE37}AI/BehaviorTrees/ConverseAction.bt";
		m_bAllowLook = true; // Disable standard looking
		m_bResetLook = true;
		InitParameters(Arsenal);
		if (!utility)
			return;
		//m_bUniqueInActionQueue = true;
		
		m_bActiveBuilding = true;
	}
	
	void SetActiveBuilding(bool state)
	{
		m_bActiveBuilding = state;
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
		if (m_bActiveBuilding)
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

class SCR_AIGetNeedParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIFulfillServiceNeedBehavior(null, null, null)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};