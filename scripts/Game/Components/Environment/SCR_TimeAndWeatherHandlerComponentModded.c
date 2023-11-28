//------------------------------------------------------------------------------------------------
modded class SCR_TimeAndWeatherHandlerComponent
{
	[Attribute("8", UIWidgets.Slider, "Starting time of day (hours)", "0 31 1")]
	protected int m_iStartingDay;

	[Attribute("8", UIWidgets.Slider, "Starting time of day (minutes)", "0 12 1")]
	protected int m_iStartingMonth;
	
	[Attribute("1989", UIWidgets.Slider, "Starting time of day (minutes)", "0 9999 1")]
	protected int m_iStartingYear;
	//------------------------------------------------------------------------------------------------
	void SetupDate(int Day, int Month, int Year)
	{
		ChimeraWorld world = ChimeraWorld.CastFrom(GetOwner().GetWorld());
		if (!world)
			return;
		
		TimeAndWeatherManagerEntity manager = world.GetTimeAndWeatherManager();
		if (!manager)
			return;

		manager.SetDate(m_iStartingYear, m_iStartingMonth, m_iStartingDay, true);
	}
	//------------------------------------------------------------------------------------------------
	override void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);

		SetupDate(m_iStartingDay, m_iStartingMonth, m_iStartingYear);
	}
}
