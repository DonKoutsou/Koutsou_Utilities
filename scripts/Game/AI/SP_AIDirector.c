class SP_AIDirectorClass: SCR_AIGroupClass
{
};
class SP_AIDirector : SCR_AIGroup
{
	static ref array<SP_AIDirector> AllDirectors = null;
	//--------------------------------------------------------------------------//
	/////////////////////////AI spawn settings///////////////////////////////////
	[Attribute("40", UIWidgets.ComboBox, "Select Entity Catalog type for random spawn", "", ParamEnumArray.FromEnum(EEntityCatalogType), category: "Randomization")]
	EEntityCatalogType	m_eEntityCatalogType;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to optionally include to random spawn. If you want to spawn everything, you can leave it out empty and also leave Include Only Selected Labels attribute to false.", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	ref array<EEditableEntityLabel> 		m_aIncludedEditableEntityLabels;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to exclude from random spawn", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	ref array<EEditableEntityLabel> 		m_aExcludedEditableEntityLabels;
	
	[Attribute("", category: "Randomization")]
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
	
	private float m_fRespawnPeriod;
	
	[Attribute("6", category: "Spawning settings")]
	int m_iDirectorUpdatePeriod;
	
	[Attribute("1", category: "Debug")]
	bool m_bVisualize;
	
	[Attribute("1")]
	bool m_bAllowCloseSpawning
	
	float m_fUpdateTimer;

	protected ResourceName m_pCharToSpawn;
	
	ref array<IEntity> m_aQueriedPrefabSpawnP;

	bool Initialised;
	
	ref array<SCR_AIGroup> m_aGroups = new array<SCR_AIGroup>();
	AIWaypoint DefWaypoint;
	private SCR_ScenarioFrameworkSlotAI m_ParentSlot;
	
	
	void SetParentSlot( SCR_ScenarioFrameworkSlotAI ParentSlot )
	{
		m_ParentSlot = ParentSlot;
	}
	void GetParentSlot( out SCR_ScenarioFrameworkSlotAI ParentSlot )
	{
		ParentSlot = m_ParentSlot;
	}
	void ClearEmptyGroups()
	{
		array <int> toremove = new array <int>();
		for (int i = m_aGroups.Count() - 1; i >= 0; i--)
		{
				if (m_aGroups[i] == null)
				{
					toremove.Insert(i);
				}
		}
		foreach(int index : toremove)
		{
			m_aGroups.Remove(index);
		}
	}
	void SP_AIDirector(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.ACTIVE, false);	
		// allow AI spawning only on the server
		if (RplSession.Mode() != RplMode.Client)
			SetEventMask(EntityEvent.FRAME);
	}
	
	void ~SP_AIDirector()
	{
		if(AllDirectors)
			AllDirectors.RemoveItem(this);
		if(m_aQueriedPrefabSpawnP)
			m_aQueriedPrefabSpawnP.Clear();
		if(m_aGroups)
			m_aGroups.Clear();
	}	
	void Init()
	{
		if(!GetGame().GetWorld())
		{
			return;
		}
		if(!AllDirectors)
			AllDirectors = new ref array<SP_AIDirector>();
		
		AllDirectors.Insert(this);
		
		m_fRespawnPeriod = Math.RandomFloat(1, 20);
		SpawnPrefab();
		Initialised = true;
	}
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!Initialised)
			return;
		if (m_fUpdateTimer > 0)
		{
			m_fUpdateTimer = m_fUpdateTimer -timeSlice;
			return;
		}
			
		m_fUpdateTimer = Math.RandomFloat(m_iDirectorUpdatePeriod*0.5, m_iDirectorUpdatePeriod + m_iDirectorUpdatePeriod*0.5);
		
		if (!m_bSpawnAI)
			return;
		//if(!m_pCharToSpawn)
		//{
		//	SetCharToSpawn();
		//}
		if (m_bRespawn)
		{
			if (m_aGroups.Count() < m_iMaxAgentsToSpawn)
			{
				if (m_fRespawnPeriod <= 0.0)
				{
					IEntity entity = m_ParentSlot.GetOwner();
					if (!m_FactionsToApear.IsEmpty())
						m_ParentSlot.SetFactionKey(m_FactionsToApear.GetRandomElement());
					m_ParentSlot.SetEntityCatalogueType(m_eEntityCatalogType);
					m_ParentSlot.ClearEntityLabels();
					m_ParentSlot.ClearPrefabtoSpawn();
					m_ParentSlot.ClearRandomPrefab();
					m_ParentSlot.SetEntityLabels(m_aIncludedEditableEntityLabels, m_aExcludedEditableEntityLabels);
					m_ParentSlot.SelectRandomSlot();
					m_ParentSlot.SetSpawnedEnt(m_ParentSlot.SpawnAsset());
					m_ParentSlot.InitEnt();
					m_ParentSlot.SetWPGroup();
					OnSpawn(m_ParentSlot.GetSpawnedEntity());
				}
				else
				{
					m_fRespawnPeriod -= m_iDirectorUpdatePeriod;
				}	
			}
		}
	}
	
	void SetCharToSpawn()
	{
		// randomize entity template from array
		FactionManager factionManager = GetGame().GetFactionManager();
		SCR_Faction randfaction = SCR_Faction.Cast(factionManager.GetFactionByKey(m_FactionsToApear.GetRandomElement()));
		SCR_EntityCatalog entityCatalog = randfaction.GetFactionEntityCatalogOfType(m_eEntityCatalogType);
		array<SCR_EntityCatalogEntry> aFactionEntityEntry = new array<SCR_EntityCatalogEntry>();
		entityCatalog.GetFullFilteredEntityListWithLabels(aFactionEntityEntry, m_aIncludedEditableEntityLabels, m_aExcludedEditableEntityLabels, false);
		m_pCharToSpawn = aFactionEntityEntry.GetRandomElement().GetPrefab();
	}

	event void OnSpawn(IEntity spawned)
	{
		SCR_AIGroup group = SCR_AIGroup.Cast(spawned);
		if (!group)
		{
			AIControlComponent contcomp = AIControlComponent.Cast(spawned.FindComponent(AIControlComponent));
			AIAgent agent = contcomp.GetAIAgent();
			if (agent)
			{
				group = SCR_AIGroup.Cast(agent.GetParentGroup());
			}
		}
		if (group)
		{
			m_aGroups.Insert(group);
			group.GetOnEmpty().Insert(ClearEmptyGroups);
		}
	}

	override protected void CreateUnitEntities(bool editMode, array<ResourceName> entityResourceNames)
	{
		if (!GetGame().GetAIWorld())
		{
			//print(string.Format("Cannot spawn team members of group %1, AIWorld is missing in the world!", this), LogLevel.WARNING);
			return;
		}
		
		if (!editMode)
		{
			//--- Get AI components
			AIFormationComponent AIFormation = AIFormationComponent.Cast(FindComponent(AIFormationComponent));
			if (!AIFormation)
			{
				//Print(string.Format("Group %1 does not have AIFormationComponent! Team members will not be spawned.", this), LogLevel.WARNING);
				return;
			}
			AIFormationDefinition formationDefinition = AIFormation.GetFormation();
			if (!formationDefinition)
			{
				//Print(string.Format("Formation of group %1 not found in SCR_AIWorld! Team members will not be spawned.", this), LogLevel.WARNING);
				return;
			}
		}
		
		//--- Apply global override
		//bool snapToTerrain = m_bSnapToTerrain;
		//if (s_bIgnoreSnapToTerrain)
		//{
		//	snapToTerrain = false;
		//	s_bIgnoreSnapToTerrain = false;
		//}
		if (Replication.IsClient())
			return;
		
		//--- We are in WB, prepare array so previews can be deleted later
		if (editMode && !m_aSceneGroupUnitInstances)
			m_aSceneGroupUnitInstances = new ref array<IEntity>;
		
		 m_iNumOfMembersToSpawn = Math.Min(entityResourceNames.Count(), m_iMaxUnitsToSpawn);
		//--- Create group members
		for (int i = m_iNumOfMembersToSpawn-1; i >= 0; i--)
		{
			// Spawn group across multiple frames
			SCR_AIGroup_DelayedSpawn delaySpawn = new SCR_AIGroup_DelayedSpawn();
			///delaySpawn.snapToTerrain	= snapToTerrain;
			delaySpawn.index			= i;
			delaySpawn.resourceName		= entityResourceNames[i];
			delaySpawn.editMode			= editMode;
			
			m_delayedSpawnList.Insert(delaySpawn);
		}

		if (editMode)
		{
			//--- Edit mode has no game world, spawn immediately
			SpawnAllImmediately();
		}
		else
		{
			//--- Enable the frame event and frames when paused
			BeginDelayedSpawn();
		}

		//--- Call group init if it cannot be called by the last spawned entity
		if (m_iNumOfMembersToSpawn == 0)
			Event_OnInit.Invoke(this);
	}
	private bool QueryEntitiesForPrefabSpawner(IEntity e)
	{
		SCR_PrefabSpawnPoint PSpawnP = SCR_PrefabSpawnPoint.Cast(e);
		//if (PSpawnP && PSpawnP.GetType() == EPrefabSpawnType.MilitaryVehicles)
		//	m_aQueriedPrefabSpawnP.Insert(e);
		if (PSpawnP)
			m_aQueriedPrefabSpawnP.Insert(e);
		
		return true;
	}
	private void GetPrefabSpawns(float radius)
	{
		BaseWorld world = GetWorld();
		world.QueryEntitiesBySphere(GetOrigin(), radius, QueryEntitiesForPrefabSpawner);
	}
	private void _CapturePrefabSpawns()
	{
		m_aQueriedPrefabSpawnP = {};
		GetPrefabSpawns(m_fRadius);
	}
	private void SpawnPrefab()
	{
		_CapturePrefabSpawns();
		if(m_aQueriedPrefabSpawnP.IsEmpty())
		{
			Print("No prefab spawners found bitch");
			return;
		}
		FactionManager factionManager = GetGame().GetFactionManager();
		if(!factionManager)
		{
			return;
		}
		int count = m_aQueriedPrefabSpawnP.Count();
		int vehcount;
		int stashcount;
		for (int i = 0; i < count; i++)
		{
			SCR_PrefabSpawnPoint Pspawn = SCR_PrefabSpawnPoint.Cast(m_aQueriedPrefabSpawnP.GetRandomElement());
			ResourceName prefab;
			if(Pspawn.GetType() == EPrefabSpawnType.MilitaryVehicles && vehcount < m_iMaxVehiclesToSpawn)
			{
				SCR_Faction randfaction = SCR_Faction.Cast(factionManager.GetFactionByKey(m_FactionsToApear.GetRandomElement()));
				SCR_EntityCatalog entityCatalog = randfaction.GetFactionEntityCatalogOfType(EEntityCatalogType.VEHICLE);
				array<SCR_EntityCatalogEntry> aFactionEntityEntry = new array<SCR_EntityCatalogEntry>();
				entityCatalog.GetEntityList(aFactionEntityEntry);
				prefab = aFactionEntityEntry.GetRandomElement().GetPrefab();
				vehcount += 1;
				EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
				vector spawnpos[4];
				Pspawn.GetWorldTransform(spawnpos);
				Resource prefabtospawn = Resource.Load(prefab);
				if (Pspawn.ShouldSnapToGound())
					SCR_TerrainHelper.SnapAndOrientToTerrain(spawnpos, GetGame().GetWorld());
				PrefabspawnParams.Transform = spawnpos;
				IEntity Entity = GetGame().SpawnEntityPrefab(prefabtospawn, GetGame().GetWorld(), PrefabspawnParams);
				SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
				if(aiWorld)
					aiWorld.RequestNavmeshRebuildEntity(Entity);
			}
			if(Pspawn.GetType() == EPrefabSpawnType.Generic && stashcount < m_iMaxStashesToSpawn)
			{
				SCR_EntityCatalogManagerComponent CatalogM = SCR_EntityCatalogManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_EntityCatalogManagerComponent));
				SCR_EntityCatalog entityCatalog = CatalogM.GetEntityCatalogOfType(EEntityCatalogType.STASH);
				array<SCR_EntityCatalogEntry> aEntityEntry = new array<SCR_EntityCatalogEntry>();
				entityCatalog.GetEntityList(aEntityEntry);
				prefab = aEntityEntry.GetRandomElement().GetPrefab();
				stashcount += 1;
				EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
				vector spawnpos[4];
				Pspawn.GetWorldTransform(spawnpos);
				Resource prefabtospawn = Resource.Load(prefab);
				if (Pspawn.ShouldSnapToGound())
					SCR_TerrainHelper.SnapAndOrientToTerrain(spawnpos, GetGame().GetWorld());
				PrefabspawnParams.Transform = spawnpos;
				IEntity Entity = GetGame().SpawnEntityPrefab(prefabtospawn, GetGame().GetWorld(), PrefabspawnParams);
			}
			m_aQueriedPrefabSpawnP.RemoveItem(Pspawn);
			delete Pspawn;
		}
		
		m_aQueriedPrefabSpawnP.Clear();
	}

	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		//if (m_bVisualize)
		//{
			string factionstospawn;
			foreach(FactionKey disfact: m_FactionsToApear)
			{
				factionstospawn = factionstospawn +  "|" + disfact + "| ";
			}
			string infoText2 = string.Format("\nMax Agents to Spawn: %1 ", m_iMaxAgentsToSpawn.ToString());
			auto origin = GetOrigin();
			auto radiusShape = Shape.CreateSphere(COLOR_BLUE, ShapeFlags.WIREFRAME | ShapeFlags.ONCE, origin, m_fRadius);	
			DebugTextWorldSpace.Create(GetGame().GetWorld(), factionstospawn + infoText2, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, origin[0], origin[1] + m_fRadius +10, origin[2], 10, 0xFFFFFFFF, Color.BLACK);

			if(m_aQueriedPrefabSpawnP && m_bVisualize)
			{
				foreach (IEntity entity : m_aQueriedPrefabSpawnP)
				{
					SCR_PrefabSpawnPoint PSpawnP = SCR_PrefabSpawnPoint.Cast(entity);
					int color;
					if(PSpawnP.GetType() == EPrefabSpawnType.MilitaryVehicles)
						color = Color.GREEN;
					else
						color = Color.ORANGE;
					vector entorigin = PSpawnP.GetOrigin();
					vector dir = entorigin;
					dir[1] = dir[1] + 5;
					Shape.CreateArrow(dir, entorigin, 2.0, color, ShapeFlags.ONCE|ShapeFlags.NOZBUFFER);
					//string SmartText = "Spawner:" + typename.EnumToString(EPrefabSpawnType, PSpawnP.GetType());
					//DebugTextWorldSpace.Create(GetGame().GetWorld(), SmartText, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, entorigin[0], entorigin[1] + 5, entorigin[2], 10, 0xFFFFFFFF, Color.BLACK);
					
				}
			}
		//}
		
		super._WB_AfterWorldUpdate(timeSlice);
	}
	private bool QueryEntitiesForCharacter(IEntity e)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(e);
		if (char)
			return false;

		return true;
	}
	private bool CheckForCharacters(float radius, vector origin)
	{
		BaseWorld world = GetGame().GetWorld();
		bool found = GetGame().GetWorld().QueryEntitiesByAABB(origin + vector.One * -128, origin + vector.One * 128, QueryEntitiesForCharacter);
		return found;
	}
};
modded class SCR_PrefabSpawnPoint
{
	void SCR_PrefabSpawnPoint(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.TRACEABLE, false);
		SCR_PrefabsSpawnerManager.RegisterPrefabSpawnPoint(this);
	}
}