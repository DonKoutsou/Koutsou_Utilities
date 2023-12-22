class SP_StartingScreenUI : ChimeraMenuBase
{
	ref array <Faction> m_aFactions = {};
	ref map <Faction, int> m_mGoodwillMap;
	
	protected Widget 						m_wRoot;
	protected ButtonWidget			m_wCloseButton;
	protected CheckBoxWidget		m_wSurvivalCheckbox;
	protected CheckBoxWidget		m_wHardcorCheckbox;
	protected CheckBoxWidget		m_wPPCheckbox;
	protected EditBoxWidget			m_wCharNameOverride;
	protected EditBoxWidget			m_wCharSurnameOverride;
	
	string name;
	string surname;
	void Init()
	{
		ChimeraWorld world = GetGame().GetWorld();

		m_wRoot = GetRootWidget();

		
		m_wCloseButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Close"));
		m_wSurvivalCheckbox = CheckBoxWidget.Cast(m_wRoot.FindAnyWidget("Survival_CheckBox"));
		m_wHardcorCheckbox = CheckBoxWidget.Cast(m_wRoot.FindAnyWidget("Hardcore_CheckBox"));
		m_wPPCheckbox = CheckBoxWidget.Cast(m_wRoot.FindAnyWidget("PP_CheckBox"));
		m_wCharNameOverride = EditBoxWidget.Cast(m_wRoot.FindAnyWidget("CharNameOverride"));
		m_wCharSurnameOverride = EditBoxWidget.Cast(m_wRoot.FindAnyWidget("CharSurnameOverride"));
		ScriptInvoker onPressedClose = ButtonActionComponent.GetOnAction(m_wCloseButton);
		if (onPressedClose)
			onPressedClose.Insert(CloseStartingScreen);

		GetGame().GetInputManager().AddActionListener("Escape", EActionTrigger.DOWN, CloseStartingScreen);

	}
	void CloseStartingScreen()
	{
		SCR_RespawnSystemComponent respawn = SCR_RespawnSystemComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_RespawnSystemComponent));
		if (respawn)
		{
			SCR_AutoSpawnLogic logic = SCR_AutoSpawnLogic.Cast(respawn.GetSpawnLogic());
			if (!m_wHardcorCheckbox.IsChecked())
			{
				logic.SetLives(-1);
				logic.SetTaskMarkers(!m_wHardcorCheckbox.IsChecked());
				logic.Set3PV(false);
			}
			else
			{
				logic.SetLives(1);
			}
			if (!m_wSurvivalCheckbox.IsChecked())
			{
				logic.OnMetDissabled();
			}
			else
			{
				logic.OnMetEnabled();
			}
			if (m_wPPCheckbox.IsChecked())
			{
				logic.EnablePost();
			}
			
			if (m_wCharNameOverride.GetText() != "Custom Name" || m_wCharNameOverride.GetText() != "STRING_EMPTY")
			{
				name = m_wCharNameOverride.GetText();
			}
			if (m_wCharSurnameOverride.GetText() != "Custom Surname" || m_wCharSurnameOverride.GetText() != "STRING_EMPTY")
			{
				surname = m_wCharSurnameOverride.GetText();
			}
			logic.SetName(name, surname);
		}
		
		Close();
	}
	
};
modded enum ChimeraMenuPreset
{
	StartingScreenMenu,
}