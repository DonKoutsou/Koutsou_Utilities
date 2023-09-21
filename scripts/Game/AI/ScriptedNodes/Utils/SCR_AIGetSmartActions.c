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
	ref array <Managed> correctsmartacts;
	
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius. Optional bool `Close Spawn` will cause to check for nearby Ai on 25m radius before selecting the smart action";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		//Get position, radius and tags in. Check if Close spawn should be enabled
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		if (!GetVariableIn(CLOSESPAWN_PORT, CloseSpawn))
			CloseSpawn = m_bAllowCloseSpawning;
		
		//if tags are empty use tags from Attribute
		if ( tags.IsEmpty() &&  m_aTags )
			tags.Copy( m_aTags ) ;
		
		//if missing radius take one from Attribute
		if ( ! Radius )
			Radius = m_fRadius ;
		
		//if missing origin get it from owner
		if ( ! Origin )
			Origin = owner.GetControlledEntity().GetOrigin() ;
		
		//if missing any of those again fail
		if ( ! Origin || ! Radius || tags.IsEmpty() )
			return ENodeResult.FAIL;
		
		//look for smart actions
		correctsmartacts = new array <Managed>();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		
		//if none fail
		if (correctsmartacts.IsEmpty())
			return ENodeResult.FAIL;
		
		//Grab random one
		OutSmartAction = SCR_AISmartActionComponent.Cast(correctsmartacts.GetRandomElement());
		
		//if close spawn is off
		if (!CloseSpawn)
		{
			//while there are Preceivable tags out there keep itterating
			ChimeraWorld world = owner.GetWorld();
			TagManager Tagmanager = world.GetTagManager();
			array <IEntity> entities = {};
			Tagmanager.GetTagsInRange(entities, OutSmartAction.GetOwner().GetOrigin(), 25, ETagCategory.NameTag);
			//start itterating though the smart action we callected and if at any point entities are clear break out of the foreach
			foreach (Managed SmartAct : correctsmartacts)
			{
				if (entities.IsEmpty())
					break;
				entities.Clear();
				OutSmartAction = SCR_AISmartActionComponent.Cast(SmartAct);
				Tagmanager.GetTagsInRange(entities, OutSmartAction.GetOwner().GetOrigin(), 25, ETagCategory.NameTag);
					
			}
		}
		//clear array
		correctsmartacts.Clear();
		//if we have a smart action return success
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
		}
		return true;
	}

	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT, CLOSESPAWN_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}