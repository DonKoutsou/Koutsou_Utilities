//simple script for lighting fireplces

modded class SCR_LightFireplaceUserAction 
{
	override bool CanBePerformedScript(IEntity user) 
	{
		TimeAndWeatherManagerEntity TnWman = ChimeraWorld.CastFrom(GetGame().GetWorld()).GetTimeAndWeatherManager();
		ref LocalWeatherSituation Weather = new LocalWeatherSituation();
		TnWman.TryGetCompleteLocalWeather(Weather, 0, GetOwner().GetOrigin());
		float rainint = Weather.GetRainIntensity();
		if (rainint > 0.05)
		{
			m_sCannotPerformReason = "Currently Raining";
			return false;
		}
		return true;
	};
};
