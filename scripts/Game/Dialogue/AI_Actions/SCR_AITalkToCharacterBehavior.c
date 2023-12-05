class SCR_AITalkToCharacterBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<IEntity> m_vEntity = new SCR_BTParam<IEntity>("Entity");
	protected ref SCR_BTParam<float> m_fDuration = new SCR_BTParam<float>("Duration");	// Initialize in derived class
	protected ref SCR_BTParam<float> m_fRadius = new SCR_BTParam<float>("Radius");		// Initialize in derived class
	protected ref SCR_BTParam<bool> m_bUseRadio = new SCR_BTParam<bool>("UseRadio");		// Initialize in derived class
	protected ref SCR_BTParam<bool> m_bAllowLeave = new SCR_BTParam<bool>("AllowLook");		// Initialize in derived class
	
	//protected ref SCR_BTParam<bool> m_bUseBinoculars = new SCR_BTParam<bool>("UseBinoculars"); // Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	bool m_buseradio;
	IEntity Owner;
	
	bool m_bActiveConversation = false;
	
	ref DS_DialogueArchetype m_orArch;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(IEntity Char, bool AllowLeave)
	{
		m_vEntity.Init(this, Char);
		m_fDuration.Init(this, 5000);
		m_fRadius.Init(this, 0);
		m_bUseRadio.Init(this, m_buseradio);
		m_bAllowLeave.Init(this, AllowLeave);
		//m_bUniqueInActionQueue = false;
		//m_bUseBinoculars.Init(this, false);
	}
	
	// posWorld - position to observe
	void SCR_AITalkToCharacterBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity Char, bool useradio, DS_DialogueArchetype Arch = null, bool AllowLeave = 1)
	{
		InitParameters(Char, AllowLeave);
		m_buseradio = useradio;
		m_sBehaviorTree = "{D3CB46095A5E9CB4}AI/BehaviorTrees/FindAndTalk.bt";
		m_bAllowLook = true; // Disable standard looking
		m_bResetLook = true;
		if (!utility)
			return;
		Owner =  AIAgent.Cast(utility.GetOwner()).GetControlledEntity();
		if (Arch)
		{
			SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Owner.FindComponent(SCR_CharacterIdentityComponent));
			if (id.HasArchetype())
			{
				m_orArch = id.GetArchetype();
			}
			id.SetCarArch(Arch);
		}
		
		
		if (!utility)
			return;
		//m_bUniqueInActionQueue = true;
		
		m_bActiveConversation = true;
	}
	
	void SetActiveConversation(bool state)
	{
		if (m_orArch && !state)
		{
			SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Owner.FindComponent(SCR_CharacterIdentityComponent));
			if (id.HasArchetype())
			{
				id.SetCarArch(m_orArch);
				m_orArch = null;
			}
		}
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
	static ref TStringArray s_aVarsOut = (new SCR_AITalkToCharacterBehavior(null, null, null, 0, null, 1)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};
class SCR_AILeadCharacterOutOfBaseBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<IEntity> m_vEntity = new SCR_BTParam<IEntity>("Entity");
	protected ref SCR_BTParam<vector> m_vPos = new SCR_BTParam<vector>("BasePos");
	protected ref SCR_BTParam<float> m_vBaseRange = new SCR_BTParam<float>("BaseRange");
	
	//protected ref SCR_BTParam<bool> m_bUseBinoculars = new SCR_BTParam<bool>("UseBinoculars"); // Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	bool m_buseradio;
	IEntity Owner;
	
	bool m_bActiveConversation = false;
	
	ref DS_DialogueArchetype m_orArch;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(IEntity Char, vector Baseloc, float baserange)
	{
		m_vEntity.Init(this, Char);
		m_vPos.Init(this, Baseloc);
		m_vBaseRange.Init(this, baserange);
		//m_bUniqueInActionQueue = false;
		//m_bUseBinoculars.Init(this, false);
	}
	
	// posWorld - position to observe
	void SCR_AILeadCharacterOutOfBaseBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity Char, vector Baseloc, int baserange)
	{
		InitParameters(Char, Baseloc, baserange);
		m_sBehaviorTree = "{2EB905511BD6186E}AI/BehaviorTrees/AI_LeadCharOutOfBase.bt";
		
		
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
class SCR_AIGetLeadOutParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AILeadCharacterOutOfBaseBehavior(null, null, null, "0 0 0", 0)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};