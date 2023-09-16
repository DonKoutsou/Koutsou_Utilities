//Checks if owner has any kid of needs to fulfill and looks for traders around
class SCR_AIEvaluateAndFindSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	protected bool m_bForceNodeSuccess;
	
	[Attribute("StorePost")]
	protected string m_sStoreTag;
	
	[Attribute()]
	float m_fRadius;
	
	[Attribute("300")]
	float m_fStoreSearchRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	protected static const string OUT_TAG_PORT = "OutTag";
	protected static const string OUT_CROUCH_BOOL = "CROUCH_BOOL";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radious;
	bool CloseSpawn;
	IEntity m_Owner;
	ref array <string> tags = {};
	ref array <Managed> correctsmartacts = {};
	ref array <string> outtags = {};
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius, evaluate either character has any needs if yes look for traders if not look for tags forom Attribute.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		//make sure arrays are empty for new cycle
		correctsmartacts.Clear();
		tags.Clear();
		outtags.Clear();
		OutSmartAction = null;
		
		
		//Get chimera
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast( owner.GetControlledEntity() );
		
		//Gather needs
		array <ERequestRewardItemDesctiptor> Needs = {};
		ERequestRewardItemDesctiptor desc;
		char.GetAllNeeds(Needs);
		
		//Gt origin of search
		GetVariableIn( POISSITION_PORT , Origin );
		
		//Get Owner Entity
		m_Owner = owner.GetControlledEntity();
		
		//if no owner fail
		if (!m_Owner)
			return ENodeResult.FAIL;
		
		//If no input for origin get origin from owner
		if ( ! Origin )
			Origin = m_Owner.GetOrigin();
		
		//if no origin still fail
		if ( ! Origin )
		{
			if( m_bForceNodeSuccess )
				return ENodeResult.SUCCESS;
			
			return ENodeResult.FAIL;
		}
		//if there are needs
		if (!Needs.IsEmpty())
		{
			//add store tag in tags to search
			tags.Insert(m_sStoreTag);
			//apply radius for seatch
			Radious = m_fStoreSearchRadius;
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			//Clear tags so that next seach doesent look for store again
			tags.Clear();
		}
		//if correctsmartacts it means that char either has no needs or cant find store for needs so look for the normal tags
		if (correctsmartacts.IsEmpty())
		{
			//apply radius for seatch
			Radious = m_fRadius;
			//Get tags from Attribute
			tags.Copy(m_aTags);
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			//if none fail
			if (correctsmartacts.IsEmpty())
			{
				if(m_bForceNodeSuccess)
					return ENodeResult.SUCCESS;
				return ENodeResult.FAIL;
			}
		}
		
		float dist;
		//find out the closest one out of found smart action
		foreach (Managed SmartA : correctsmartacts)
		{
			
			SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast( SmartA );
			//set up first distance
			if ( ! dist )
			{
				dist = vector.Distance( Smart.m_Owner.GetOrigin() , owner.GetControlledEntity().GetOrigin() );
				OutSmartAction = Smart;
			}
			// if any of the next smart actions has smaller distance and set it as the one to use
			else if ( dist > vector.Distance( Smart.m_Owner.GetOrigin() , owner.GetControlledEntity().GetOrigin() ) )
			{
				dist = vector.Distance( Smart.m_Owner.GetOrigin() , owner.GetControlledEntity().GetOrigin() );
				OutSmartAction = Smart;
			}
		}
		//if we have one get its variables out
		if (OutSmartAction)
		{
			//if char should crouch when using action
			SetVariableOut( OUT_CROUCH_BOOL, OutSmartAction.ShouldCrouchWhenPerforming );
			//smart action
			SetVariableOut( SMARTACTION_PORT , OutSmartAction );
			OutSmartAction.GetTags(outtags);
			//smart action tags
			SetVariableOut(OUT_TAG_PORT, outtags);
			return ENodeResult.SUCCESS;
		}
		if(m_bForceNodeSuccess)
			return ENodeResult.SUCCESS;
		return ENodeResult.FAIL;
	}
	//special querry looks for smart action, doese the test on them to determine if "can be performed"
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		
		//find smart action component
		e.FindComponents(SCR_AISmartActionComponent, smartacts);
		
		//if none go to next entity
		if (smartacts.IsEmpty())
			return true;
		
		else
		{
			foreach (Managed Smart : smartacts)
			{
				//Check if action is accessible
				SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				
				//Get tags of smart action
				array <string> demtags2 = {};
				smatcomp.GetTags( demtags2 );
				
				
				foreach (string tg : demtags2)
				{
					//check if tags of action are what we are looking for
					if (tags.Contains(tg))
					{
						//if action needs test run it and if it succeds add it to array if not add it straight to the array
						if (smatcomp.NeedsTest())
						{
							if (smatcomp.RunTest(m_Owner))
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
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT, OUT_TAG_PORT ,OUT_CROUCH_BOOL};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
	