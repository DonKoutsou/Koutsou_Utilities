class WJ_StaminaBarUI: SCR_InfoDisplay
{
	private ProgressBarWidget m_wStaminaBar = null;
	private ProgressBarWidget m_wHungerBar = null;
	private ProgressBarWidget m_wThirstBar = null;
	private TextWidget m_sTempnumber = null;
	private ProgressBarWidget m_wTemp = null;
	private SCR_CharacterControllerComponent m_cCharacterController = null;
	private SP_CharacterStatsComponent CharStats = null;

	void OnStaminaChange(float value)
	{
		if (!m_wStaminaBar)
		{
			m_wStaminaBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wStaminaBar"));
			if (!m_wStaminaBar) return;
		};
		
		m_wStaminaBar.SetCurrent(value);
	}
	void OnHungerChange(float value)
	{
		if (!m_wHungerBar)
		{
			m_wHungerBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wHungerBar"));
			if (!m_wHungerBar) return;
		};
		
		m_wHungerBar.SetCurrent(value);
	}
	void OnThirstChange(float value)
	{
		if (!m_wThirstBar)
		{
			m_wThirstBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wThirstBar"));
			if (!m_wThirstBar) return;
		};
		
		m_wThirstBar.SetCurrent(value);
	}
	void OnTempChange(float value)
	{
		float whateva;
		float r = 0;
		float b = 0;
		if (!m_wTemp)
		{
			m_wTemp = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wTemp"));
			if (!m_wTemp) return;
		};
		float temp = Math.InverseLerp(29, 42, value);
		r = 1 * temp;
		b = 1-1 * temp;
		//if(value > 36.6)
		//{
		//	whateva = value - 36.6;
		//	r = 1 * temp;
		//	Color mycolor = m_wTemp.GetColor();
		//	m_wTemp.SetColor(mycolor);
		//}
		//if(value < 36.6)
		//{
		//	whateva = value - 36.6;
		//	b = 1 * temp;
		//	Color mycolor = Color(r, 0, b, 1);
		//	m_wTemp.SetColor(mycolor);
		//}
		Color mycolor = Color(r, 0, b, 1);
		m_wTemp.SetColor(mycolor);
		string val = value.ToString().Substring(0, 4);
		string tempnumber = val + "C";
		m_sTempnumber.SetText(tempnumber);
		m_wTemp.SetCurrent(value);
		
	}
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		m_wTemp = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wTemp"));
		m_wStaminaBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wStaminaBar"));
		m_wHungerBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wHungerBar"));
		m_wThirstBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wThirstBar"));
		m_sTempnumber = TextWidget.Cast(m_wRoot.FindAnyWidget("m_sTempnumber"));
		IEntity player = GetGame().GetPlayerController();
		if (!player) {
			Print("no player found");
			return;
		}
		m_cCharacterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		if (m_cCharacterController) Print("Found character controllerc omponent");
		CharStats = SP_CharacterStatsComponent.Cast(player.FindComponent(SP_CharacterStatsComponent));
		SCR_PlayerController cont = SCR_PlayerController.Cast(owner);
		if (!cont)
			return;
		cont.m_OnControlledEntityChanged.Insert(UpdateChar);
	}
	void UpdateChar(IEntity from, IEntity to)
	{
		if (!to)
			return;
		m_cCharacterController = SCR_CharacterControllerComponent.Cast(GetGame().GetPlayerController().FindComponent(SCR_CharacterControllerComponent));
		CharStats = SP_CharacterStatsComponent.Cast(GetGame().GetPlayerController().FindComponent(SP_CharacterStatsComponent));
	}
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		if (!m_cCharacterController)
		{
			IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			if (!player) {
				return;
			}
		
			m_cCharacterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
			if (!m_cCharacterController) return;
		};
		
		OnStaminaChange(m_cCharacterController.GetStamina());
		if(!CharStats)
		{
			IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			if (!player) {
				return;
			}
			CharStats = SP_CharacterStatsComponent.Cast(player.FindComponent(SP_CharacterStatsComponent));
			if (!m_cCharacterController) return;
		}
		OnHungerChange(CharStats.GetHunger());
		OnThirstChange(CharStats.GetThirst());
		OnTempChange(CharStats.GetCurrentTemp());
		
	}
}