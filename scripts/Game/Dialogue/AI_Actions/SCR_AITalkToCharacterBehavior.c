class SCR_AITalkToCharacterBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<IEntity> m_vEntity = new SCR_BTParam<IEntity>("Entity");
	protected ref SCR_BTParam<float> m_fDuration = new SCR_BTParam<float>("Duration");	// Initialize in derived class
	protected ref SCR_BTParam<float> m_fRadius = new SCR_BTParam<float>("Radius");		// Initialize in derived class
	protected ref SCR_BTParam<bool> m_bUseRadio = new SCR_BTParam<bool>("UseRadio");		// Initialize in derived class
	
	//protected ref SCR_BTParam<bool> m_bUseBinoculars = new SCR_BTParam<bool>("UseBinoculars"); // Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	bool m_buseradio;
	IEntity Owner;
	
	bool m_bActiveConversation = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(IEntity Char)
	{
		m_vEntity.Init(this, Char);
		m_fDuration.Init(this, 5000);
		m_fRadius.Init(this, 0);
		m_bUseRadio.Init(this, m_buseradio);
		//m_bUniqueInActionQueue = false;
		//m_bUseBinoculars.Init(this, false);
	}
	
	// posWorld - position to observe
	void SCR_AITalkToCharacterBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity Char, bool useradio)
	{
		
		
		m_buseradio = useradio;
		m_sBehaviorTree = "{D3CB46095A5E9CB4}AI/BehaviorTrees/FindAndTalk.bt";
		m_bAllowLook = true; // Disable standard looking
		m_bResetLook = true;
		InitParameters(Char);
		if (!utility)
			return;
		//m_bUniqueInActionQueue = true;
		
		m_bActiveConversation = true;
	}
	
	void SetActiveConversation(bool state)
	{
		m_bActiveConversation = state;
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
		if (m_bActiveConversation)
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

class SCR_AIGetTalkParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIConverseBehavior(null, null, null, 0)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};