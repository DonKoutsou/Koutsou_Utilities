modded class TimeAndWeatherManagerEntity
{	
	[Attribute("8")]
	float m_fDayTemp;
	
	[Attribute("2")]
	float m_fNightTemp;
	
	float CalculateFeelsLikeTemperature(IEntity entity)
	{
		float timeOfDay = GetTimeOfTheDay();
		float sunrise, sunset;
		GetSunriseHour(sunrise);
		GetSunsetHour(sunset);	
			
		float outsideTemperature = m_fDayTemp;
		if (timeOfDay < sunrise || timeOfDay > sunset)
			outsideTemperature = m_fNightTemp;
		float alt = GetPlayerAltitude(entity) * 0.0065;
		outsideTemperature -=  alt;
	   // Wind chill formula for temperatures above 10 degrees Celsius
	   //if (outsideTemperature > 10.0)
	       //return outsideTemperature;
	   // Wind chill formula for temperatures below 10 degrees Celsius
	   float windChillIndex = 13.12 + 0.6215 * outsideTemperature - 11.37 * Math.Pow(GetWindSpeed(), 0.16) + 0.3965 * outsideTemperature * Math.Pow(GetWindSpeed(), 0.16);

	   // Add an adjustment for rain
	   windChillIndex = windChillIndex - 0.25 * GetRainIntensity() * (windChillIndex - 10.0);
			
		//altitude
		
	   return windChillIndex;
	}
	float GetPlayerAltitude(IEntity Player)
	{
		vector pos = Player.GetOrigin();
		float alt = pos[1];
		return alt;
	}
}