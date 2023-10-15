class SCR_AIGetClosestSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	protected bool m_bForceNodeSuccess;
	
	[Attribute()]
	float m_fRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	protected static const string OUT_TAG_PORT = "OutTag";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radius;
	bool CloseSpawn;
	IEntity Owner;
	ref array <string> tags = new ref array <string>();
	ref array <Managed> correctsmartacts;
	ref array <string> outtags = new ref array <string>();
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
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		Owner = owner.GetControlledEntity();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		if (correctsmartacts.IsEmpty())
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		float dist;
		foreach (Managed SmartA : correctsmartacts)
		{
			SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast(SmartA);
			
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
			outtags.Clear();
			OutSmartAction.GetTags(outtags);
			SetVariableOut(OUT_TAG_PORT, outtags);
			correctsmartacts.Clear();
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		
		if(m_bForceNodeSuccess)
			return ENodeResult.SUCCESS;
		return ENodeResult.FAIL;
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		if (!correctsmartacts)
		{
			correctsmartacts = {};
		}
		e.FindComponents(SCR_AISmartActionComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		else
		{
			foreach (Managed Smart : smartacts)
			{
				array <string> demtags2 = new array <string>();
				SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				
				smatcomp.GetTags(demtags2);
				foreach (string tg : demtags2)
				{
					if (tags.Contains(tg))
					{
						if (smatcomp.NeedsTest())
						{
							if (smatcomp.RunTest(Owner))
								correctsmartacts.Insert(Smart);
						}
						else
							correctsmartacts.Insert(Smart);
					}
						
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT, OUT_TAG_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}