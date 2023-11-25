[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
modded class DS_DialogueStage
{
	[Attribute(desc: "Event to be recorded on notebook")]
	protected ref BaseEventContainer m_sEventString;

	//------------------------------------------------------------------//
	bool GetEvent(out string eventString)
	{
		if (!m_DialogueActionCondition)
			return false;
		else
		{
			m_DialogueActionCondition.GetCannotPerformReason(eventString);
			return true;
		}
	}
	//------------------------------------------------------------------//
	override void Perform(IEntity Character, IEntity Player)
	{
		super.Perform(Character, Player);
		if (m_iTimesPerformed > 1)
			return;
		if (m_sEventString)
		{
			SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(Player);
			SP_CallendarComponent Callendar = Char.GetCallendar();
			if (Callendar)
			{
				Callendar.RecordEvent(m_sEventString.GetString(Player, Character));
			}
		}
		
	};
}