modded class SCR_ScenarioFrameworkLayerBase
{	
	[Attribute("0", desc: "Weather randomisation should use faction key from area or get players.", category: "Asset")]
	bool						m_bUsePlayerFaction;
	override  FactionKey GetFactionKey()
	{
		return m_sFactionKey;
	}
	override  void SetFactionKey(FactionKey factionKey)
	{
		m_sFactionKey = factionKey;
	}
}

