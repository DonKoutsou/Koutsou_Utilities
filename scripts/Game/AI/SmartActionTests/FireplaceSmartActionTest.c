class FireplaceSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		if(!m_LightComponent.IsOn())
		{
			return true;
		}
		return false;
	}
}