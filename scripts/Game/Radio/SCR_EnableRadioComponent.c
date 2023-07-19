
class SCR_EnableRadioComponentClass: ScriptComponentClass
{
};

class SCR_EnableRadioComponent: ScriptComponent
{
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}	
	
	override void EOnInit(IEntity owner)
	{
		RadioBroadcastComponent radio = RadioBroadcastComponent.Cast(owner.FindComponent(RadioBroadcastComponent));
		if (radio != null)
		{
			radio.EnableRadio(true);
		}

	}
};