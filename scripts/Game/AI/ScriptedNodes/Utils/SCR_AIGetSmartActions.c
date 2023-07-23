//! Returns a position of entity with local space offset
class SCR_AIGetSmartAction : AITaskScripted
{
	[Attribute("0 0 0", UIWidgets.Auto)]
	protected vector m_fOffsetLocal;
	
	[Attribute("0 0 0", UIWidgets.Auto)]
	protected vector m_fOffsetWorld;
	
	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string CLOSESPAWN_PORT = "AllowCloseSpawn";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radius;
	bool CloseSpawn;
	ref array <string> tags = new ref array <string>();
	
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Returns position of entity with local and world space offset.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		GetVariableIn(CLOSESPAWN_PORT, CloseSpawn);
		if (!Origin || !Radius || tags.IsEmpty())
			return ENodeResult.FAIL;
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		
		if (OutSmartAction)
		{
			SetVariableOut(SMARTACTION_PORT, OutSmartAction);
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		
		return ENodeResult.FAIL;
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		e.FindComponents(SCR_AISmartActionComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		if (!tags.IsEmpty())
		{
			array <string> demtags = new array <string>();
			foreach (Managed Smart : smartacts)
			{
				SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
				smatcomp.GetTags(demtags);
			}
				
			foreach (string tag : demtags)
			{
				if (tags.Contains(tag))
				{
					break;
				}
				else
				{
					return true;
				}
			}
		}
		if (!CloseSpawn)
		{
			bool found = GetGame().GetWorld().QueryEntitiesBySphere(e.GetOrigin(), 50, QueryEntitiesForCharacter);
			if (!found)
				return true;
		}
		foreach (Managed Smart : smartacts)
		{
			SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
			if (smatcomp.IsActionAccessible())
			{
				OutSmartAction = smatcomp;
				return false;
			}
		}
		return true;
	}
	private bool QueryEntitiesForCharacter(IEntity e)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(e);
		if (!char)
			return true;
		return false;
	}

	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT, CLOSESPAWN_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}