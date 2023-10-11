[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginDirector: SCR_ScenarioFrameworkPlugin
{
	[Attribute("40", UIWidgets.ComboBox, "Select Entity Catalog type for random spawn", "", ParamEnumArray.FromEnum(EEntityCatalogType), category: "Randomization")]
	protected EEntityCatalogType	m_eEntityCatalogType;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to optionally include to random spawn. If you want to spawn everything, you can leave it out empty and also leave Include Only Selected Labels attribute to false.", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	protected ref array<EEditableEntityLabel> 		m_aIncludedEditableEntityLabels;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to exclude from random spawn", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	protected ref array<EEditableEntityLabel> 		m_aExcludedEditableEntityLabels;
	
	[Attribute("", category: "Randomization", desc: "If this has entries faction will be randomised on spawn. If no entries faction from slot will be taken")]
	ref array<FactionKey> m_FactionsToApear;

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
	
	[Attribute("10", category: "Spawning settings")]
	int m_iMaxStashesToSpawn;
	
	[Attribute("100", category: "Spawning settings")]
	float m_fRadius;
	
	[Attribute("0", category: "Spawning settings")]
	float m_fRespawnTimer;
	
	[Attribute("6", category: "Spawning settings")]
	int m_iDirectorUpdatePeriod;
	
	[Attribute("1", category: "Debug")]
	protected bool m_bVisualize;
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		if (!object)
			return;

		super.Init(object);
		SP_AIDirector director;
		IEntity entity = object.GetSpawnedEntity();
		
		SCR_ScenarioFrameworkSlotAI Parentslot = SCR_ScenarioFrameworkSlotAI.Cast(object);
		
		director = SP_AIDirector.Cast(entity);
		
		if (director)
		{
			director.m_eEntityCatalogType = m_eEntityCatalogType;
			director.m_aIncludedEditableEntityLabels.Copy(m_aIncludedEditableEntityLabels);
			director.m_aExcludedEditableEntityLabels.Copy(m_aExcludedEditableEntityLabels);
			director.m_bSpawnAI = m_bSpawnAI;
			director.m_bRespawn = m_bRespawn;
			director.m_iMaxAgentsToSpawn = m_iMaxAgentsToSpawn;
			director.m_iMaxVehiclesToSpawn = m_iMaxVehiclesToSpawn;
			director.m_iMaxStashesToSpawn = m_iMaxStashesToSpawn;
			director.m_fRadius = m_fRadius;
			director.m_fRespawnTimer = m_fRespawnTimer;
			director.m_iDirectorUpdatePeriod = m_iDirectorUpdatePeriod;
			director.m_bVisualize = m_bVisualize;
			if (!m_FactionsToApear.IsEmpty())
			{
				director.m_FactionsToApear = m_FactionsToApear;
			}
			else
			{
				director.m_FactionsToApear.Clear();
				director.m_FactionsToApear.Insert(Parentslot.GetFactionKey());
			}
			
			director.SetParentSlot(Parentslot);
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