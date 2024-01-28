class SCR_AIBuildPathBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<array <LightPost>> m_vEntity = new SCR_BTParam<array <LightPost>>("Path");
	
	//protected ref SCR_BTParam<bool> m_bUseBinoculars = new SCR_BTParam<bool>("UseBinoculars"); // Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	IEntity Owner;
	
	bool m_bActiveBuilding = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(array <LightPost> path)
	{
		m_vEntity.Init(this, path);
		//m_bUniqueInActionQueue = false;
		//m_bUseBinoculars.Init(this, false);
	}
	
	// posWorld - position to observe
	void SCR_AIBuildPathBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, array <LightPost> path)
	{
		m_sBehaviorTree = "{D65DC09987CECE37}AI/BehaviorTrees/ConverseAction.bt";
		m_bAllowLook = true; // Disable standard looking
		m_bResetLook = true;
		InitParameters(path);
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

class SCR_AIGetPathParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIBuildPathBehavior(null, null, null)).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};
class SCR_AIGetClosestPost : AITaskScripted
{
	protected const static string POST = "LightPost";
	protected const static string PATH = "Path";
	
	
	protected static ref TStringArray s_aVarsIn = {PATH};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = {POST};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		array <LightPost> posts = {};
		LightPost ClosestPost;
		if (!GetVariableIn(PATH, posts))
			return ENodeResult.FAIL;
		IEntity ent = owner.GetControlledEntity();
		if (!ent)
			return ENodeResult.FAIL;
		float dist;
		for (int i; i < posts.Count(); i++)
		{
			if (!dist)
			{
				dist = vector.Distance(posts[i].GetOrigin(), ent.GetOrigin());
				ClosestPost = posts[i];
			}
			else if (dist > vector.Distance(posts[i].GetOrigin(), ent.GetOrigin()) && !posts[i].IsBuilt())
			{
				dist = vector.Distance(posts[i].GetOrigin(), ent.GetOrigin());
				ClosestPost = posts[i];
			}
		}
		if (ClosestPost)
		{
			SetVariableOut(POST, ClosestPost);
			return ENodeResult.SUCCESS;
		}
		return ENodeResult.FAIL;
	}
	override bool VisibleInPalette() { return true; }
	
}
class SCR_AIGetNextUnbuiltPost : AITaskScripted
{
	protected const static string POST = "LightPost";
	protected const static string PATH = "Path";
	
	
	protected static ref TStringArray s_aVarsIn = {PATH};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = {POST};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		array <LightPost> posts = {};
		LightPost ClosestPost;
		if (!GetVariableIn(PATH, posts))
			return ENodeResult.FAIL;
		IEntity ent = owner.GetControlledEntity();
		if (!ent)
			return ENodeResult.FAIL;

		for (int i; i < posts.Count(); i++)
		{
			if (!posts[i].IsBuilt())
			{
				ClosestPost = posts[i];
				break;
			}
		}
		if (ClosestPost)
		{
			SetVariableOut(POST, ClosestPost);
			return ENodeResult.SUCCESS;
		}
		return ENodeResult.FAIL;
	}
	override bool VisibleInPalette() { return true; }
	
}