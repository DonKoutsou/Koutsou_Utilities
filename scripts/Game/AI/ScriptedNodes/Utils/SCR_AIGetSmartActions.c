class SCR_AIGetSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	float m_fRadius;
	
	[Attribute()]
	bool m_bAllowCloseSpawning;
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
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		if (!GetVariableIn(CLOSESPAWN_PORT, CloseSpawn))
			CloseSpawn = m_bAllowCloseSpawning;
		if (tags.IsEmpty() && m_aTags)
			tags.Copy(m_aTags);
		if (!Radius)
			Radius = m_fRadius;
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
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
		array <Managed> correctsmartacts = new array <Managed>();
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
						correctsmartacts.Insert(Smart);
				}
			}
			if (correctsmartacts.IsEmpty())
				return true;
		}
		
		if (!CloseSpawn)
		{
			bool found = GetGame().GetWorld().QueryEntitiesBySphere(e.GetOrigin(), 25, QueryEntitiesForCharacter);
			if (!found)
				return true;
		}
		OutSmartAction = SCR_AISmartActionComponent.Cast(correctsmartacts.GetRandomElement());
		return false;
	}
	private bool QueryEntitiesForCharacter(IEntity e)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(e);
		if (!char)
			return true;
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(e.FindComponent(ScriptedDamageManagerComponent));
		if (dmg.GetState() == EDamageState.DESTROYED)
			return true;
		return false;
	}

	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT, CLOSESPAWN_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}