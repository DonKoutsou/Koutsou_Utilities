//Checks if owner has any kid of needs to fulfill and looks for traders around
class SCR_AIEvaluateAndFindSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute("StorePost")]
	protected string m_sStoreTag;
	
	[Attribute()]
	float m_fRadius;
	
	[Attribute("300")]
	float m_fStoreSearchRadius;
	
	[Attribute(defvalue : "GatePost")]
	string m_sGateTag;
	
	[Attribute(defvalue : "LoiterPost")]
	string m_sIdleActionTag;

	protected static const string CHARACTER_PORT = "Character";
	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "InRadius";
	protected static const string INTAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionComponent OutSmartAction;
	
	bool CloseSpawn;
	IEntity m_Owner;
	ref array <string> tags = {};
	ref array <Managed> a_CorrectSmartActs = {};
	ref array <Managed> a_GateSmartActions = {};
	ref array <Managed> a_IdleSmartActions = {};
	ref array <string> outtags = {};
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius, evaluate either character has any needs if yes look for traders if not look for tags forom Attribute.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		//make sure arrays are empty for new cycle
		a_CorrectSmartActs.Clear();
		tags.Clear();
		outtags.Clear();
		OutSmartAction = null;
		
		vector Origin;
		float Radious;
		//Get chimera
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast( owner.GetControlledEntity() );
		if (!char)
		{
			AIAgent Agent;
			GetVariableIn( CHARACTER_PORT , Agent );
			char = SCR_ChimeraCharacter.Cast(Agent.GetControlledEntity());
		}
		
		//Gather needs
		array <ERequestRewardItemDesctiptor> Needs = {};
		ERequestRewardItemDesctiptor desc;
		char.GetAllNeeds(Needs);
		
		//Gt origin of search
		GetVariableIn( POISSITION_PORT , Origin );
		
		//Gt radius of search
		GetVariableIn( RADIUS_PORT , Radious );
		if (!Radious)
			Radious = m_fStoreSearchRadius;
		
		
		
		//Get Owner Entity
		m_Owner = char;
		
		//if no owner fail
		if (!m_Owner)
			return ENodeResult.FAIL;
		
		//If no input for origin get origin from owner
		if ( ! Origin )
			Origin = m_Owner.GetOrigin();
		
		//if no origin still fail
		if ( ! Origin )
		{
			return ENodeResult.FAIL;
		}
		//if there are needs
		if (!Needs.IsEmpty())
		{
			//add store tag in tags to search
			tags.Insert(m_sStoreTag);
			
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			//Clear tags so that next seach doesent look for store again
			tags.Clear();
		}
		//if a_CorrectSmartActs it means that char either has no needs or cant find store for needs so look for the normal tags
		if (a_CorrectSmartActs.IsEmpty())
		{
			//apply radius for seatch
			GetVariableIn( INTAGS_PORT , tags );
			if (tags.IsEmpty())
			{
				//Get tags from Attribute
				tags.Copy(m_aTags);
			}
			
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			float dist;
			//find out the closest one out of found smart action
			if (!a_CorrectSmartActs.IsEmpty())
			{
				foreach (Managed SmartA : a_CorrectSmartActs)
				{
					dist = 0;
					SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast( SmartA );
					//set up first distance
					if ( ! dist )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
					// if any of the next smart actions has smaller distance and set it as the one to use
					else if ( dist > vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() ) )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
				}
			}
			else if (!a_GateSmartActions.IsEmpty())
			{
				dist = 0;
				foreach (Managed SmartA : a_GateSmartActions)
				{
					SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast( SmartA );
					//set up first distance
					if ( ! dist )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
					// if any of the next smart actions has smaller distance and set it as the one to use
					else if ( dist > vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() ) )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
				}
			}
			else if (!a_IdleSmartActions.IsEmpty())
			{
				dist = 0;
				foreach (Managed SmartA : a_IdleSmartActions)
				{
					SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast( SmartA );
					if (!Smart)
						continue;
					//set up first distance
					if ( ! dist )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
					// if any of the next smart actions has smaller distance and set it as the one to use
					else if ( dist > vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() ) )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
				}
			}
		}
		//if we have one get its variables out
		if (OutSmartAction)
		{
			//SP_StoreAISmartActionComponent storeaction = SP_StoreAISmartActionComponent.Cast(OutSmartAction);
			//if (storeaction)
			//	OutSmartAction = SCR_AISmartActionComponent.Cast(a_CorrectSmartActs.GetRandomElement());
			//if char should crouch when using action
			//smart action
			SetVariableOut( SMARTACTION_PORT , OutSmartAction );
			return ENodeResult.SUCCESS;
		}
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
								a_CorrectSmartActs.Insert(Smart);
						}
						else
							a_CorrectSmartActs.Insert(Smart);
					}
					else if (tg == m_sGateTag)
					{
						a_GateSmartActions.Insert(Smart);
					}
					else if (tg == m_sIdleActionTag)
					{
						a_IdleSmartActions.Insert(Smart);
					}
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = {CHARACTER_PORT, POISSITION_PORT, RADIUS_PORT, INTAGS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SCR_AIEvaluateAndFindSmartActionJob : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute("StorePost")]
	protected string m_sStoreTag;
	
	[Attribute()]
	float m_fRadius;
	
	[Attribute("300")]
	float m_fStoreSearchRadius;
	

	protected static const string CHARACTER_PORT = "Character";
	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "InRadius";
	protected static const string INTAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionComponent OutSmartAction;
	
	bool CloseSpawn;
	IEntity m_Owner;
	ref array <string> tags = {};
	ref array <Managed> a_CorrectSmartActs = {};
	ref array <string> outtags = {};
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius, evaluate either character has any needs if yes look for traders if not look for tags forom Attribute.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		//make sure arrays are empty for new cycle
		a_CorrectSmartActs.Clear();
		tags.Clear();
		outtags.Clear();
		OutSmartAction = null;
		
		vector Origin;
		float Radious;
		//Get chimera
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast( owner.GetControlledEntity() );
		if (!char)
		{
			AIAgent Agent;
			GetVariableIn( CHARACTER_PORT , Agent );
			char = SCR_ChimeraCharacter.Cast(Agent.GetControlledEntity());
		}
		
		//Gather needs
		array <ERequestRewardItemDesctiptor> Needs = {};
		ERequestRewardItemDesctiptor desc;
		char.GetAllNeeds(Needs);
		
		//Gt origin of search
		GetVariableIn( POISSITION_PORT , Origin );
		
		//Gt radius of search
		GetVariableIn( RADIUS_PORT , Radious );
		if (!Radious)
			Radious = m_fRadius;
		
		
		
		//Get Owner Entity
		m_Owner = char;
		
		//if no owner fail
		if (!m_Owner)
			return ENodeResult.FAIL;
		
		//If no input for origin get origin from owner
		if ( ! Origin )
			Origin = m_Owner.GetOrigin();
		
		//if no origin still fail
		if ( ! Origin )
		{
			return ENodeResult.FAIL;
		}
		//if there are needs
		if (!Needs.IsEmpty())
		{
			//add store tag in tags to search
			tags.Insert(m_sStoreTag);
			
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			//Clear tags so that next seach doesent look for store again
			tags.Clear();
		}
		//if a_CorrectSmartActs it means that char either has no needs or cant find store for needs so look for the normal tags
		if (a_CorrectSmartActs.IsEmpty())
		{
			//apply radius for seatch
			GetVariableIn( INTAGS_PORT , tags );
			if (tags.IsEmpty())
			{
				//Get tags from Attribute
				tags.Copy(m_aTags);
			}
			
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			float dist;
			//find out the closest one out of found smart action
			if (!a_CorrectSmartActs.IsEmpty())
			{
				OutSmartAction = SCR_AISmartActionComponent.Cast(a_CorrectSmartActs.GetRandomElement());
				/*foreach (Managed SmartA : a_CorrectSmartActs)
				{
					dist = 0;
					SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast( SmartA );
					//set up first distance
					if ( ! dist )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
					// if any of the next smart actions has smaller distance and set it as the one to use
					else if ( dist > vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() ) )
					{
						dist = vector.Distance( Smart.m_Owner.GetOrigin() , m_Owner.GetOrigin() );
						OutSmartAction = Smart;
					}
				}*/
			}
		}
		//if we have one get its variables out
		if (OutSmartAction)
		{
			//SP_StoreAISmartActionComponent storeaction = SP_StoreAISmartActionComponent.Cast(OutSmartAction);
			//if (storeaction)
			//	OutSmartAction = SCR_AISmartActionComponent.Cast(a_CorrectSmartActs.GetRandomElement());
			//if char should crouch when using action
			//smart action
			SetVariableOut( SMARTACTION_PORT , OutSmartAction );
			return ENodeResult.SUCCESS;
		}
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
								a_CorrectSmartActs.Insert(Smart);
						}
						else
							a_CorrectSmartActs.Insert(Smart);
					}
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = {CHARACTER_PORT, POISSITION_PORT, RADIUS_PORT, INTAGS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
//Checks if owner has any kid of needs to fulfill and looks for traders around
class SCR_AIEvaluateAndFindSmartActionV2 : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute("StorePost")]
	protected string m_sStoreTag;
	
	[Attribute()]
	float m_fRadius;
	
	[Attribute("300")]
	float m_fStoreSearchRadius;


	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	protected static const string OUT_TAG_PORT = "OutTag";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radious;
	bool CloseSpawn;
	IEntity m_Owner;
	ref array <string> tags = {};
	ref array <Managed> a_CorrectSmartActs = {};
	ref array <string> outtags = {};
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius, evaluate either character has any needs if yes look for traders if not look for tags forom Attribute.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		//make sure arrays are empty for new cycle
		a_CorrectSmartActs.Clear();
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
		//if a_CorrectSmartActs it means that char either has no needs or cant find store for needs so look for the normal tags
		if (a_CorrectSmartActs.IsEmpty())
		{
			if (!GetVariableIn( RADIUS_PORT , Radious ))
			{
				//apply radius for seatch
				Radious = m_fRadius;
			}
			
			//Get tags from Attribute
			tags.Copy(m_aTags);
			//Do the query
			GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
			float dist;
			//find out the closest one out of found smart action
			if (!a_CorrectSmartActs.IsEmpty())
			{
				foreach (Managed SmartA : a_CorrectSmartActs)
				{
					dist = 0;
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
			}
		}
		//if we have one get its variables out
		if (OutSmartAction)
		{
			//SP_StoreAISmartActionComponent storeaction = SP_StoreAISmartActionComponent.Cast(OutSmartAction);
			//if (storeaction)
				//OutSmartAction = SCR_AISmartActionComponent.Cast(a_CorrectSmartActs.GetRandomElement());
			//smart action
			SetVariableOut( SMARTACTION_PORT , OutSmartAction );
			OutSmartAction.GetTags(outtags);
			//smart action tags
			SetVariableOut(OUT_TAG_PORT, outtags);
			return ENodeResult.SUCCESS;
		}
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
								a_CorrectSmartActs.Insert(Smart);
						}
						else
							a_CorrectSmartActs.Insert(Smart);
					}
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT, RADIUS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT, OUT_TAG_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
	
	