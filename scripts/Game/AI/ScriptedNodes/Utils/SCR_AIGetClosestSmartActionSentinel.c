class SCR_AIGetClosestSmartActionSentinel : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	float m_fRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionSentinelComponent OutSmartAction;
	vector Origin;
	float Radius;
	bool CloseSpawn;
	IEntity Owner;
	ref array <string> tags = new ref array <string>();
	ref array <Managed> correctsmartacts;
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		if (tags.IsEmpty() && m_aTags)
			tags.Copy(m_aTags);
		if (!Radius)
			Radius = m_fRadius;
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
		if (!Origin || !Radius || tags.IsEmpty())
			return ENodeResult.FAIL;
		Owner = owner.GetControlledEntity();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		if (correctsmartacts.IsEmpty())
			return ENodeResult.FAIL;
		float dist;
		foreach (Managed SmartA : correctsmartacts)
		{
			SCR_AISmartActionSentinelComponent Smart = SCR_AISmartActionSentinelComponent.Cast(SmartA);
			
			if (!dist)
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
			else if (dist > vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin()))
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
		}
		
		if (OutSmartAction)
		{
			SetVariableOut(SMARTACTION_PORT, OutSmartAction);
			correctsmartacts.Clear();
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		
		return ENodeResult.FAIL;
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		if (!correctsmartacts)
		{
			correctsmartacts = {};
		}
		e.FindComponents(SCR_AISmartActionSentinelComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		else
		{
			foreach (Managed Smart : smartacts)
			{
				array <string> demtags2 = new array <string>();
				SCR_AISmartActionSentinelComponent smatcomp = SCR_AISmartActionSentinelComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				smatcomp.GetTags(demtags2);
				foreach (string tg : demtags2)
				{
					if (tags.Contains(tg))
						correctsmartacts.Insert(Smart);
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}