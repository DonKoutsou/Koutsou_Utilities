class SCR_AISayHi: AITaskScripted
{
	
	static const string PORT_SPEAKER = "SpeakerIn";

	//-----------------------------------	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
  {
		IEntity Speaker; 
		GetVariableIn(PORT_SPEAKER, Speaker);
		if (!Speaker)
			return ENodeResult.FAIL;
		FactionAffiliationComponent aff = FactionAffiliationComponent.Cast(Speaker.FindComponent(FactionAffiliationComponent));
		string eventname = SCR_Faction.Cast(aff.GetAffiliatedFaction()).GetGreetName();
		CharacterSoundComponent soundc = CharacterSoundComponent.Cast(Speaker.FindComponent(CharacterSoundComponent));
		soundc.SoundEvent(eventname);
		
		return ENodeResult.SUCCESS;
	}
	//----------------------------------------------------------------------
	override bool VisibleInPalette()
    {
        return true;
    }
	
	//----------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		PORT_SPEAKER,
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
	}
	
	
	//----------------------------------------------------------------------
	override protected bool CanReturnRunning()
	{
		return true;
	}
	
	//----------------------------------------------------------------------
	protected void _print(string str)
	{
		Print(string.Format("SCR_AITalk %1: %2", this, str));
	}
};
class SP_AITalk: AITaskScripted
{
	[Attribute(defvalue: "Voice_HM")]
	string m_sSoundEventName;
	
	[Attribute()]
	string m_DialogTexttoshow;
	
	
	
	static const string PORT_SPEAKER = "SpeakerIn";

	//-----------------------------------	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
  {
		IEntity Speaker; 
		GetVariableIn(PORT_SPEAKER, Speaker);
		if (!Speaker)
		{
			Speaker = owner.GetControlledEntity();
		}
		if (!Speaker)
			return ENodeResult.FAIL;

		CharacterSoundComponent soundc = CharacterSoundComponent.Cast(Speaker.FindComponent(CharacterSoundComponent));
		soundc.SoundEvent(m_sSoundEventName);
		
		if (m_DialogTexttoshow)
		{
			array<BaseInfoDisplay> infoDisplays = {};
			GetGame().GetPlayerController().GetHUDManagerComponent().GetInfoDisplays(infoDisplays);
			foreach (BaseInfoDisplay baseInfoDisplays : infoDisplays)
			{
				SCR_DialogueWidget DialogueDisplay = SCR_DialogueWidget.Cast(baseInfoDisplays);
				if (!DialogueDisplay)
					continue;
		
				DialogueDisplay.SetTarget(Speaker);
				DialogueDisplay.SetText(m_DialogTexttoshow);
				DialogueDisplay.ShowInspectCasualtyWidget(Speaker);
			}
		}
		
		return ENodeResult.SUCCESS;
	}
	//----------------------------------------------------------------------
	override bool VisibleInPalette()
    {
        return true;
    }
	
	//----------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		PORT_SPEAKER,
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
	}
	
	
	//----------------------------------------------------------------------
	override protected bool CanReturnRunning()
	{
		return true;
	}
	
	//----------------------------------------------------------------------
	protected void _print(string str)
	{
		Print(string.Format("SCR_AITalk %1: %2", this, str));
	}
}
class SP_AISetPlayerFactionEnemy: AITaskScripted
{
	//-----------------------------------	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
  {
		IEntity Player = GetGame().GetPlayerController().GetControlledEntity();
		if (!Player)
			return ENodeResult.FAIL;
		
		IEntity Char = owner.GetControlledEntity();
		
		FactionAffiliationComponent PlAff = FactionAffiliationComponent.Cast(Player.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent CHAff = FactionAffiliationComponent.Cast(Char.FindComponent(FactionAffiliationComponent));
		
		SCR_CampaignFactionManager facman = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		facman.SetFactionsHostile(SCR_Faction.Cast(PlAff.GetAffiliatedFaction()), SCR_Faction.Cast(CHAff.GetAffiliatedFaction()));
		
		return ENodeResult.SUCCESS;
	}
	//----------------------------------------------------------------------
	override bool VisibleInPalette()
    {
        return true;
    }
	
	//----------------------------------------------------------------------
	override protected bool CanReturnRunning()
	{
		return true;
	}
	
	//----------------------------------------------------------------------
	protected void _print(string str)
	{
		Print(string.Format("SCR_AITalk %1: %2", this, str));
	}
}
