class RadioSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		RadioBroadcastComponent m_RadioComponent = RadioBroadcastComponent.Cast(Owner.FindComponent(RadioBroadcastComponent));
		return m_RadioComponent.GetState();
	}
}