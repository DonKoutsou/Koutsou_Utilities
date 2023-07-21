[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginDirector: SCR_ScenarioFrameworkPlugin
{
	[Attribute("40", UIWidgets.ComboBox, "Select Entity Catalog type for random spawn", "", ParamEnumArray.FromEnum(EEntityCatalogType), category: "Randomization")]
	protected EEntityCatalogType	m_eEntityCatalogType;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to optionally include to random spawn. If you want to spawn everything, you can leave it out empty and also leave Include Only Selected Labels attribute to false.", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	protected ref array<EEditableEntityLabel> 		m_aIncludedEditableEntityLabels;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to exclude from random spawn", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	protected ref array<EEditableEntityLabel> 		m_aExcludedEditableEntityLabels;
	
	[Attribute(desc: "If true, it will spawn only the entities that are from Included Editable Entity Labels and also do not contain Label to be Excluded.", category: "Randomization")]
	protected bool				m_bIncludeOnlySelectedLabels;

	//--------------------------------------------------------------------------//
	// parameters
	[Attribute("true", category: "Spawning settings")]
	bool m_bSpawnAI;
	
	[Attribute("true", category: "Spawning settings")]
	bool m_bRespawn;
	
	[Attribute("3", category: "Spawning settings")]
	int m_iMaxAgentsToSpawn;
	
	[Attribute("10", category: "Spawning settings")]
	int m_iMaxVehiclesToSpawn;
	
	[Attribute("100", category: "Spawning settings")]
	float m_fRadius;
	
	[Attribute("0", category: "Spawning settings")]
	float m_fRespawnTimer;
	
	[Attribute("6", category: "Spawning settings")]
	int m_iDirectorUpdatePeriod;
	
	[Attribute("{93291E72AC23930F}prefabs/AI/Waypoints/AIWaypoint_Defend.et", category: "Spawning settings")]
	private ResourceName m_pDefaultWaypoint;
	
	[Attribute("1", category: "Debug")]
	protected bool m_bVisualize;
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		if (!object)
			return;

		super.Init(object);
		SP_AIDirector director;
		IEntity entity = object.GetSpawnedEntity();
		
		SCR_ScenarioFrameworkSlotBase slot = SCR_ScenarioFrameworkSlotBase.Cast(object);
		
		director = SP_AIDirector.Cast(entity);
		
		if (director)
		{
			director.m_eEntityCatalogType = m_eEntityCatalogType;
			director.m_aIncludedEditableEntityLabels = m_aIncludedEditableEntityLabels;
			director.m_aExcludedEditableEntityLabels = m_aExcludedEditableEntityLabels;
			director.m_bIncludeOnlySelectedLabels = m_bIncludeOnlySelectedLabels;
			director.m_bSpawnAI = m_bSpawnAI;
			director.m_bRespawn = m_bRespawn;
			director.m_iMaxAgentsToSpawn = m_iMaxAgentsToSpawn;
			director.m_iMaxVehiclesToSpawn = m_iMaxVehiclesToSpawn;
			director.m_fRadius = m_fRadius;
			director.m_fRespawnTimer = m_fRespawnTimer;
			director.m_iDirectorUpdatePeriod = m_iDirectorUpdatePeriod;
			director.m_pDefaultWaypoint = m_pDefaultWaypoint;
			director.m_bVisualize = m_bVisualize;
			director.m_FactionsToApear.Clear();
			director.m_FactionsToApear.Insert(slot.GetFactionKey());
			director.slot = slot;
			director.Init();
		}
		
	}
	
	override void OnWBKeyChanged(SCR_ScenarioFrameworkLayerBase object) 
	{
		super.OnWBKeyChanged(object);
		object.SetDebugShapeSize(m_fRadius);
		//src.Set("m_sAreaName", m_fAreaRadius);
	}
}