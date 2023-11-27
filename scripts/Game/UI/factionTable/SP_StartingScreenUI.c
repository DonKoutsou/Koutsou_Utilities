class SP_StartingScreenUI : ChimeraMenuBase
{
	ref array <Faction> m_aFactions = {};
	ref map <Faction, int> m_mGoodwillMap;
	
	protected Widget 						m_wRoot;
	protected ButtonWidget			m_wCloseButton;
	protected CheckBoxWidget		m_wSurvivalCheckbox;
	protected CheckBoxWidget		m_wHardcorCheckbox;
	void Init()
	{
		ChimeraWorld world = GetGame().GetWorld();

		m_wRoot = GetRootWidget();

		
		m_wCloseButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Close"));
		m_wSurvivalCheckbox = CheckBoxWidget.Cast(m_wRoot.FindAnyWidget("Survival_CheckBox"));
		m_wHardcorCheckbox = CheckBoxWidget.Cast(m_wRoot.FindAnyWidget("Hardcore_CheckBox"));
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
			}
			if (!m_wSurvivalCheckbox.IsChecked())
			{
				logic.OnMetDissabled();
			}
			else
			{
				logic.OnMetEnabled();
			}
		}
		
		Close();
	}
	
};
modded enum ChimeraMenuPreset
{
	StartingScreenMenu,
}