[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginWaypoint: SCR_ScenarioFrameworkPlugin
{
	[Attribute()]
	float m_fRadius
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		if (!object)
			return;

		super.Init(object);
		AIWaypoint WP = AIWaypoint.Cast(object.GetSpawnedEntity());
		if (WP)
			WP.SetCompletionRadius(m_fRadius);
	}
	override void OnWBKeyChanged(SCR_ScenarioFrameworkLayerBase object) 
	{
		super.OnWBKeyChanged(object);
		object.SetDebugShapeSize(m_fRadius);
		//src.Set("m_sAreaName", m_fAreaRadius);
	}
}
[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginTPWaypoint: SCR_ScenarioFrameworkPlugin
{
	[Attribute()]
	float m_fRadius;
	
	[Attribute("1")]
	bool m_bAllowCloseSpawning;
	
	[Attribute("CustomPost", UIWidgets.Auto, "List tags to search in the preset")];
	ref array<string> m_aTagsForSearch;	
	
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		if (!object)
			return;

		super.Init(object);
		SP_TPWaypoint WP = SP_TPWaypoint.Cast(object.GetSpawnedEntity());
		if (WP)
		{
			WP.SetCompletionRadius(m_fRadius);
			WP.SetAllowCloseSpawning(m_bAllowCloseSpawning);
			WP.SetTagsForSearch(m_aTagsForSearch);
		}
			
			
	}
	override void OnWBKeyChanged(SCR_ScenarioFrameworkLayerBase object) 
	{
		super.OnWBKeyChanged(object);
		object.SetDebugShapeSize(m_fRadius);
		//src.Set("m_sAreaName", m_fAreaRadius);
	}
}