modded class TimeAndWeatherManagerEntity
{	
	float CalculateFeelsLikeTemperature(IEntity entity)
	{
		float timeOfDay = GetTimeOfTheDay();
		float sunrise, sunset;
		GetSunriseHour(sunrise);
		GetSunsetHour(sunset);
			
		const float dayTemp = 18.0;
		const float nightTemp = 10.0;		
			
		float outsideTemperature = dayTemp;
		if (timeOfDay < sunrise || timeOfDay > sunset)
			outsideTemperature = nightTemp;
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