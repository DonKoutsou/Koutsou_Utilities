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
	static int GetAmountOfDaysInMonth(int Month)
	{
		int ammount;
		switch (Month)
		{
			case 1 :
			{
				ammount = 31;
			}
			break;
			case 2:
			{
				ammount = 28;
			}
			break;
			case 3:
			{
				ammount = 31;
			}
			break;
			case 4:
			{
				ammount = 30;
			}
			break;
			case 5:
			{
				ammount = 31;
			}
			break;
			case 6:
			{
				ammount = 30;
			}
			break;
			case 7:
			{
				ammount = 31;
			}
			break;
			case 8:
			{
				ammount = 31;
			}
			break;
			case 9:
			{
				ammount = 30;
			}
			break;
			case 10:
			{
				ammount = 31;
			}
			break;
			case 11:
			{
				ammount = 30;
			}
			break;
			case 12:
			{
				ammount = 31;
			}
			break;
		}
		return ammount;
	}
}