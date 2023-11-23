[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
modded class DialogueStage
{
	[Attribute(desc: "Event to be recorded on notebook")]
	protected ref BaseEventContainer m_sEventString;
	
	protected bool m_bEventRecorded;
	//------------------------------------------------------------------//
	bool GetEvent(out string eventString)
	{
		if (!m_sCantBePerformedReason)
			return false;
		else
		{
			eventString = m_sCantBePerformedReason;
			return true;
		}
	}
	//------------------------------------------------------------------//
	override void Perform(IEntity Character, IEntity Player)
	{
		if (m_sEventString && !m_bEventRecorded)
		{
			SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(Player);
			SP_CallendarComponent Callendar = Char.GetCallendar();
			if (Callendar)
			{
				Callendar.RecordEvent(m_sEventString.GetString(Player, Character));
				m_bEventRecorded = true;
			}
		}
		
	};
}