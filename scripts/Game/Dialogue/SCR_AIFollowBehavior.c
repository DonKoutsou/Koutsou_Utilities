class SCR_AIFollowBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<IEntity> m_vChar = new SCR_BTParam<IEntity>("Character");
	protected ref SCR_BTParam<float> m_fDuration = new SCR_BTParam<float>("Duration");	// Initialize in derived class
	protected ref SCR_BTParam<float> m_fRadius = new SCR_BTParam<float>("Radius");		// Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	
	bool m_bActiveFollowing = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(IEntity CharToFollow, AIAgent Agent)
	{
		m_vChar.Init(this, CharToFollow);
		m_fDuration.Init(this, 5000);
		m_fRadius.Init(this, 0);
	}
	
	// posWorld - position to observe
	void SCR_AIFollowBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity CharToFollow)
	{
		if(CharToFollow)
		{
			AIControlComponent comp = AIControlComponent.Cast(CharToFollow.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			InitParameters(CharToFollow, agent);
		}
		else
		{
			InitParameters(CharToFollow, null);
		}

		
		if (!utility)
			return;
				
		m_sBehaviorTree = "{F89BF93EA3363C34}AI/BehaviorTrees/AI_Navigate_Follow.bt";
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

class SCR_AIFollowParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIFollowBehavior(null, null, null)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};

class SCR_FollowAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// how to tell AI to run behavior
		AIControlComponent comp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		SCR_AIFollowBehavior action = new SCR_AIFollowBehavior(utility, null, pUserEntity);
		utility.AddAction(action);

	}
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}	
		
	void StopConversation(IEntity pOwnerEntity)
	{
		// how to tell AI to run behavior
		AIControlComponent comp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		utility.SetStateAllActionsOfType(SCR_AIFollowBehavior, EAIActionState.FAILED, true);
		
		// or
		SCR_AIFollowBehavior action = SCR_AIFollowBehavior.Cast(utility.FindActionOfType(SCR_AIFollowBehavior));
		action.SetActiveFollowing(false);
	}
};
class DecoratorScripted_IsBiggerThan : DecoratorScripted
{
	[Attribute("0")]
	private int m_DistanceTocompareto;
	
	protected override bool TestFunction(AIAgent owner)
	{
		int var = GetVariableType(true, "value1");
		if (var >= m_DistanceTocompareto)
		{
			return true;
		}
		return false;
	}
	
	protected override bool VisibleInPalette()
	{
		return true;
	}	
	
	protected override string GetOnHoverDescription()
	{
		return "DecoratorScripted_IsEqual: Compares whether 1st variable is bigger than 2nd. Supports int-int, float-float";
	}
	
	protected static ref TStringArray s_aVarsIn = {
		"value1"
	};
	protected override TStringArray GetVariablesIn()
	{
		return s_aVarsIn;
	}
};