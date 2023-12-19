class FireplaceSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		TimeAndWeatherManagerEntity TnWman = ChimeraWorld.CastFrom(GetGame().GetWorld()).GetTimeAndWeatherManager();
		ref LocalWeatherSituation Weather = new LocalWeatherSituation();
		TnWman.TryGetCompleteLocalWeather(Weather, 0, Owner.GetOrigin());
		float rainint = Weather.GetRainIntensity();
		if (rainint > 0.05)
		{
			return false;
		}
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		if(!m_LightComponent.IsOn())
		{
			return true;
		}
		return false;
	}
}
class LightpostSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		LightPost post = LightPost.Cast(Owner);
		if (!post)
			return false;
		if (post.IsBuilt())
			return false;
		return true;
	}
}