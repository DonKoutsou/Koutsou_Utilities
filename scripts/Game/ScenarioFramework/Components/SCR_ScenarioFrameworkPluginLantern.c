[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginLantern: SCR_ScenarioFrameworkPlugin
{
	[Attribute()]
	bool m_bLight
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		SCR_BaseInteractiveLightComponent light = SCR_BaseInteractiveLightComponent.Cast(object.GetSpawnedEntity().FindComponent(SCR_BaseInteractiveLightComponent));
		if (light)
			light.ToggleLight(m_bLight);
	}
}