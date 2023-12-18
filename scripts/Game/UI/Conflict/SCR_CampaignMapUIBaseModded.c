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
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		return false;
		if (m_wImageOverlay && w == m_wImageOverlay && !m_wBaseOverlay.IsEnabled())
			m_wBaseOverlay.SetEnabled(true);	// enable the underlying button, allowing click

		SCR_UITaskManagerComponent tm = SCR_UITaskManagerComponent.GetInstance();
		if (tm && !tm.IsTaskListOpen())
		{
			GetGame().GetWorkspace().SetFocusedWidget(w);
		}

		if (m_OnMapIconEnter)
			m_OnMapIconEnter.Invoke();

		super.OnMouseEnter(w, x, y);

		if (w.Type() == ButtonWidget)
			AnimExpand();
		
		if (m_Base)
		{
			m_Base.GetMapDescriptor().OnIconHovered(this, true);

		if (m_wServices && m_Base && m_Base.GetType() == SCR_ECampaignBaseType.BASE && m_Base.IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(SCR_FactionManager.SGetLocalPlayerFaction())))
			{
				m_wServices.SetVisible(!m_mServices.IsEmpty());
				m_wServices.SetEnabled(!m_mServices.IsEmpty());
			}
		}
		
		if (m_wInfoText)
		{
			m_wInfoText.SetVisible(true);
			m_wRoot.SetZOrder(1);
		}
		
		if (m_MobileAssembly){
			m_MobileAssembly.OnIconHovered(this, true);
			m_wServices.SetVisible(true);
			m_w_ServicesOverlay.SetVisible(false);
			m_wRoot.SetZOrder(1);
		}
		
		if (!m_bCanRespawn && m_bIsRespawnMenu)
			return false;

		return false;
	}
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		return false;
		super.OnMouseLeave(w, enterW, x, y);

		AnimCollapse();
		
		if(m_wInfoText)
		{
		m_wInfoText.SetVisible(false);
		m_wRoot.SetZOrder(0);
		}
		
		if (m_Base)
			m_Base.GetMapDescriptor().OnIconHovered(this, false);

		if (m_MobileAssembly)
			m_MobileAssembly.OnIconHovered(this, false);
			m_wInfoText.SetVisible(false);

		if (!m_bCanRespawn && m_bIsRespawnMenu)
			return false;

		if (m_wServices)
			m_wServices.SetEnabled(false);

		if (m_wLocalTask.IsEnabled())
			m_wLocalTask.SetVisible(true);

		if (enterW)
			m_bCanPlaySounds = ScriptedWidgetEventHandler.Cast(w.FindHandler(SCR_CampaignMapUIService)) == null;
		else
			m_bCanPlaySounds = true;

		if (RenderTargetWidget.Cast(enterW) && m_wBaseOverlay.IsEnabled())
		{
			m_wBaseOverlay.SetEnabled(false); // disable the base widget when not hovered, deactivating the button
			m_bCanPlaySounds = true;
		}

		return false;
	}
}