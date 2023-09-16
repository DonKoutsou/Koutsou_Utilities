class AliveSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		ScriptedDamageManagerComponent DMGMan = ScriptedDamageManagerComponent.Cast(Owner.FindComponent(ScriptedDamageManagerComponent));
		if (DMGMan.IsDestroyed())
		{
			return true;
		}
		return false;
	}
}