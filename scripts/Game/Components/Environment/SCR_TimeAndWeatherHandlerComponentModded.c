//------------------------------------------------------------------------------------------------
modded class SCR_TimeAndWeatherHandlerComponent
{
	[Attribute("8", UIWidgets.Slider, "Starting time of day (hours)", "0 31 1")]
	protected int m_iStartingDay;

	[Attribute("8", UIWidgets.Slider, "Starting time of day (minutes)", "0 12 1")]
	protected int m_iStartingMonth;
	
	[Attribute("1989", UIWidgets.Slider, "Starting time of day (minutes)", "0 9999 1")]
	protected int m_iStartingYear;
	
	[Attribute("180", UIWidgets.Slider, "Starting time of day (hours)", "-180 180 1")]
	protected int m_iLongitude;

	[Attribute("90", UIWidgets.Slider, "Starting time of day (minutes)", "-90 90 1")]
	protected int m_iLatitude;
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
	void SetupLonLat(int Latitude, int Longitude)
	{
		ChimeraWorld world = ChimeraWorld.CastFrom(GetOwner().GetWorld());
		if (!world)
			return;
		TimeAndWeatherManagerEntity tnwman = world.GetTimeAndWeatherManager();
		if (!tnwman)
			return;
		tnwman.SetCurrentLatitude(Latitude);
		tnwman.SetCurrentLongitude(Longitude);
	}
	//------------------------------------------------------------------------------------------------
	override void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);

		SetupDate(m_iStartingDay, m_iStartingMonth, m_iStartingYear);
		SetupLonLat(m_iLatitude, m_iLongitude);
	}
}
