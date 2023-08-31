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
		correctsmartacts = new array <Managed>();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		if (correctsmartacts.IsEmpty())
			return ENodeResult.FAIL;
		OutSmartAction = SCR_AISmartActionComponent.Cast(correctsmartacts.GetRandomElement());
		if (!CloseSpawn)
		{
			array <IEntity> entities = {};
			GetGame().GetTagManager().GetTagsInRange(entities, OutSmartAction.GetOwner().GetOrigin(), 25, ETagCategory.Perceivable);
			while (!entities.IsEmpty())
			{
				entities.Clear();
				OutSmartAction = SCR_AISmartActionComponent.Cast(correctsmartacts.GetRandomElement());
				GetGame().GetTagManager().GetTagsInRange(entities, OutSmartAction.GetOwner().GetOrigin(), 25, ETagCategory.Perceivable);
			}
		}
		correctsmartacts.Clear();
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
class SCR_AIGetClosestSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	float m_fRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionComponent OutSmartAction;
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
				if (smatcomp.NeedsTest())
				{
					if (!smatcomp.RunTest(Owner))
						continue;
				}
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
class SCR_AIGetSmartActionPerformTest : AITaskScripted
{
	protected static const string SMART_ACTION_PORT = "Samart Action";
	protected static const string TREE_PORT = "Tree";
	override bool VisibleInPalette() { return true; }
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AISmartActionComponent smart;
		GetVariableIn(SMART_ACTION_PORT, smart);
		
		if (smart)
		{
			//SetVariableOut(SMART_ACTION_PORT, smart.GetCanBePerformedTree());
			smart = null;
			return ENodeResult.SUCCESS;
		}
		
		return ENodeResult.FAIL;
	}

	protected static ref TStringArray s_aVarsIn = { SMART_ACTION_PORT };
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { TREE_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SCR_AIGetSmartActionTagsToLook : AITaskScripted
{
	protected static const string TAGS_PORT = "TAGS";
	override bool VisibleInPalette() { return true; }
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		
		//if (smart)
		//{
		//	SetVariableOut(SMART_ACTION_PORT, smart.GetCanBePerformedTree());
		//	smart = null;
		//	return ENodeResult.SUCCESS;
		//}
		return ENodeResult.FAIL;
	}
	
	protected static ref TStringArray s_aVarsOut = { TAGS_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SmartActionTest : ScriptAndConfig
{
	bool TestAction(IEntity Owner, IEntity User)
	{
		return true;
	}
}
class LanternSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		bool LightShouldBe;
		float currenttime = GetGame().GetTimeAndWeatherManager().GetTime().ToTimeOfTheDay();
		float Sunset, Sunrise;
		GetGame().GetTimeAndWeatherManager().GetSunsetHour(Sunset);
		GetGame().GetTimeAndWeatherManager().GetSunriseHour(Sunrise);
		if (Sunset < currenttime || Sunrise > currenttime)
			LightShouldBe = true;
		
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		
		bool light = m_LightComponent.IsOn();
		if(light != LightShouldBe)
		{
			return true;
		}
		return false;
	}
}
class FireplaceSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		if(!m_LightComponent.IsOn())
		{
			return true;
		}
		return false;
	}
}
class RadioSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		RadioBroadcastComponent m_RadioComponent = RadioBroadcastComponent.Cast(Owner.FindComponent(RadioBroadcastComponent));
		return m_RadioComponent.GetState();
	}
}
class AliveSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		ScriptedDamageManagerComponent DMGMan = ScriptedDamageManagerComponent.Cast(Owner.FindComponent(ScriptedDamageManagerComponent));
		if (DMGMan.IsDestroyed())
		{
			return true;
		}
		return false;
	}
}