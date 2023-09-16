class LanternSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		bool LightShouldBe;
		float currenttime = GetGame().GetTimeAndWeatherManager().GetTime().ToTimeOfTheDay();
		float Sunset, Sunrise;
		GetGame().GetTimeAndWeatherManager().GetSunsetHour(Sunset);
		GetGame().GetTimeAndWeatherManager().GetSunriseHour(Sunrise);
		if (Sunset < currenttime || Sunrise > currenttime)
			LightShouldBe = true;
		
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		
		bool light = m_LightComponent.IsOn();
		if(light != LightShouldBe)
		{
			return true;
		}
		return false;
	}
}