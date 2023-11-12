modded class SCR_CampaignMapUIBase
{
	override protected void InitBaseIcon()
	{
		if (!m_Base)
			return;

		SCR_Faction f;
		//if (m_bIsEditor || m_Base.IsHQ() || (m_PlayerFaction && m_Base.IsHQRadioTrafficPossible(m_PlayerFaction)) || )
		//{
			f = m_Base.GetCampaignFaction();
		//}
		
		Widget w = m_wRoot.FindAnyWidget("rootWidget");
		if (w)
			FrameSlot.SetAlignment(w, 0.5, m_fNameOffset);

		SetIconFaction(f);
		SetIconName(m_Base.GetBaseName());
		SetBaseType(m_Base.GetType());
		SetBaseImage();
		ChangeBaseIconSize();
	}
}