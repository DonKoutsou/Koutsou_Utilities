modded class SCR_GameModeSFManager
{	
	[Attribute(desc: "Possible factions that can appear either as playable or enemies", category: "Debug")];
	ref array <FactionKey>	m_fFactionsToAppear;
	
	protected ref ScriptInvoker m_OnPlayerFactionSelected;
	protected SCR_BaseTask		m_ExtractionAreaTask;
	protected SCR_BaseTask		m_LastFinishedTask;
	protected SCR_ScenarioFrameworkLayerBase		m_LastFinishedTaskLayer;
	protected bool				m_bInitialized = false;
	protected bool 				m_bMatchOver;
	protected int 				m_iNumberOfTasksSpawned;
	protected int 				m_iNumberOfSelectedAreas;
	protected ref array<string> 	m_aAreasTasksToSpawn = {};
	protected ref array<string> 	m_aLayersTaskToSpawn = {};
	FactionKey m_FactionToAppear;
	
	override protected bool Init()
	{
		if (!IsMaster())
			return false;

		LoadHeaderSettings();
		
		SP_GameMode Gamemod = SP_GameMode.Cast(GetGame().GetGameMode());
		InitAreas();
		
		return true;
	}
	protected bool InitAreas()
	{
		SP_GameMode Gamemod = SP_GameMode.Cast(GetGame().GetGameMode());
 		// Spawn everything inside the Area except the task layers
		SCR_FactionManager factman = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		//m_fFactionsToAppear.RemoveItem(Gamemod.GetPlFaction().GetFactionKey());
 		foreach(SCR_ScenarioFrameworkArea area : m_aAreas)
 		{
 			if (m_sForcedArea.IsEmpty())		//for debug purposes
 			{
				if (!area.GetFactionKey())
				{
					if (area.m_bUsePlayerFaction)
					{
						area.SetFactionKey(Gamemod.GetPlFaction().GetFactionKey());
						
					}
					else
					{
						m_FactionToAppear = m_fFactionsToAppear.GetRandomElement();
						area.SetFactionKey(m_FactionToAppear);
					}
						
				}
 				area.Init();	
 			}
		// Spawn everything inside the Area except the task layers
			else
			{
				if (area.GetOwner().GetName() == m_sForcedArea)
					area.Init(area);
			}
		}	
		SCR_ScenarioFrameworkLayerTask.s_OnTaskSetup.Insert(OnTaskCreated);													  
		SCR_BaseTaskManager.s_OnTaskUpdate.Insert(OnTaskUpdate);
		
		//if someone registered for the event, then call it
		if (m_OnAllAreasInitiated)
			m_OnAllAreasInitiated.Invoke();
		
		PostInit();
		
		m_bInitialized = true;
		return true;
	}
	
}
