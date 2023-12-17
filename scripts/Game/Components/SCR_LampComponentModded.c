
modded class SCR_LampComponent
{
	override void UpdateLight()
	{
		SCR_BaseInteractiveLightComponentClass componentData = SCR_BaseInteractiveLightComponentClass.Cast(GetComponentData(GetOwner()));
		if (!componentData) 
			return;		
		
		if (m_SignalsManagerComponent)
		{
			m_fSoundSignalValue += m_fSoundSignalStep;
			m_SignalsManagerComponent.SetSignalValue(m_SignalsManagerComponent.AddOrFindSignal("Trigger"), m_fSoundSignalValue);
		}
	}
};