class SP_StaminaBarUI: SCR_InfoDisplay
{
	private ProgressBarWidget m_wStaminaBar = null;
	private ProgressBarWidget m_wHungerBar = null;
	private ProgressBarWidget m_wThirstBar = null;
	private ProgressBarWidget m_wEnergyBar = null;
	private TextWidget m_sTempnumber = null;
	private ProgressBarWidget m_wTemp = null;
	private SCR_CharacterControllerComponent m_cCharacterController = null;
	private SP_CharacterStatsComponent CharStats = null;
	private int maxtemp;
	private int mintemp;
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
	void OnEnergyChange(float value)
	{
		if (!m_wEnergyBar)
		{
			m_wEnergyBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wThirstBar"));
			if (!m_wEnergyBar) return;
		};
		Widget m_wEnergyDown = m_wRoot.FindAnyWidget("m_wEnergyDown");
			Widget m_wEnergyUp = m_wRoot.FindAnyWidget("m_wEnergyUp");
		if (value > m_wEnergyBar.GetCurrent())
		{
			m_wEnergyDown.SetOpacity(0);
			m_wEnergyUp.SetOpacity(100);
		}
		if (value < m_wEnergyBar.GetCurrent())
		{
			m_wEnergyDown.SetOpacity(100);
			m_wEnergyUp.SetOpacity(0);
		}
		m_wEnergyBar.SetCurrent(value);
	}
	void OnTempChange(float value)
	{
		float whateva;
		float r = 0;
		float b = 0;
		float g = 0;
		if (!m_wTemp)
		{
			m_wTemp = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wTemp"));
			if (!m_wTemp) return;
		};
		float temp = Math.InverseLerp(mintemp, maxtemp, value);
		
		if (temp <= 0.5) 
		{
			if (temp < 0)
			{
				r = 0;
				g = 0;
				b = 1;
			}
			else
			{
				r = temp * 2.0;
		    g = temp * 2.0;
		    b = 1 - temp / 2;
			}
		} 
		else 
		{
		    r = 1;
		    g = 1.0 - (temp - 0.5) * 2.0;
		    b = 0;
		}
		Color mycolor = new Color(r, g, b, 1);
		m_wTemp.SetColor(mycolor);
		string val = value.ToString();
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
		m_wEnergyBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wEnergyBar"));
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
		maxtemp = CharStats.m_fMaxTemperature;
		mintemp = CharStats.m_fUnconsciousTemperature;
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
		OnEnergyChange(CharStats.GetEnergy());
	}
}