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
	
	[Attribute("100", category: "Spawning settings")]
	float m_fRadius;
	
	[Attribute("0", category: "Spawning settings")]
	float m_fRespawnTimer;
	
	private float m_fRespawnPeriod;
	
	[Attribute("6", category: "Spawning settings")]
	int m_iDirectorUpdatePeriod;

	
	[Attribute("1", category: "Tasks")]
	bool m_bAllowRescue;
	
	
	[Attribute("1", category: "Debug")]
	bool m_bVisualize;
	
	[Attribute("1")]
	bool m_bAllowCloseSpawning
	
	float m_fUpdateTimer;
	
	protected vector m_vPositiontoSpawn;
	protected ResourceName m_pCharToSpawn;
	
	ref array<IEntity> m_aQueriedSentinels;
	ref array<IEntity> m_aQueriedPrefabSpawnP;
	

	bool Initialised;
	
	ref array<SCR_AIGroup> m_aGroups = new array<SCR_AIGroup>();
	AIWaypoint DefWaypoint;
	SCR_ScenarioFrameworkSlotAI slot;
	/*bool GetDirectorOccupiedByFriendly(Faction faction, out SP_AIDirector Director)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		ref array<SP_AIDirector> FrDirectors = new ref array<SP_AIDirector>;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			Faction Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if(faction.IsFactionFriendly(Key2))
			{
				if(Directors[i] != this)
				{
					FrDirectors.Insert(Directors[i]);
					UnitCount = Count;
				}
			}
		}
		if(FrDirectors.Count() <= 0)
		{
			return false;
		}	
		else
		{
			int index = Math.RandomInt(0, FrDirectors.Count());
			Director = FrDirectors[index];
			return true;
		}
		return false;
	}
	bool GetDirectorOccupiedByEnemy(Faction faction, out SP_AIDirector Director)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		ref array<SP_AIDirector> EnDirectors = new ref array<SP_AIDirector>;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			Faction Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if(faction.IsFactionEnemy(Key2))
			{
				EnDirectors.Insert(Directors[i]);
				UnitCount = Count;
			}
			
			
		}
		if(EnDirectors.Count() <= 0)
		{
			return false;
		}	
		else
		{
			int index = Math.RandomInt(0, EnDirectors.Count());
			Director = EnDirectors[index];
			return true;
		}
		return false;
	}
	bool GetDirectorOccupiedBy(FactionKey Key, out SP_AIDirector Director)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		ref array<SP_AIDirector> FrDirectors = new ref array<SP_AIDirector>;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			Faction Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if (Key2.GetFactionKey() == Key && Directors[i] != this)
			{
				FrDirectors.Insert(Directors[i]);
				UnitCount = Count;
			}
		}
		if(FrDirectors.Count() <= 0)
		{
			return false;
		}	
		else
		{
			int index = Math.RandomInt(0, FrDirectors.Count());
			Director = FrDirectors[index];
			return true;
		}
		return false;
	}
	bool GetRandomUnitByFKey(FactionKey Key, out IEntity Char)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		for (int i = m_aGroups.Count() - 1; i >= 0; i--)
		{
			string faction = m_aGroups[i].GetFaction().GetFactionKey();
			if (faction && faction == Key)
			{
				array<AIAgent> outAgents = new array<AIAgent>();
				m_aGroups[i].GetAgents(outAgents);
				if (outAgents.IsEmpty())
					return false;
				int z = Math.RandomInt(0,outAgents.Count());				
				Char = outAgents[z].GetControlledEntity();
				return true;
			}
		}
		return false;
	}
	bool GetRandomUnit(out IEntity Char)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		array<AIAgent> outAgents = new array<AIAgent>();
		SCR_AIGroup luckygroup = m_aGroups.GetRandomElement();
		if(!luckygroup)
		{
			return false;
		}
		luckygroup.GetAgents(outAgents);
		if(outAgents.Count() <= 0)
		{
			return false;
		}
		Char = outAgents.GetRandomElement().GetControlledEntity();
		if(Char)
		{
			return true;
		}
		return false;
	}
	Faction GetMajorityHolderNCount(out string factionReadable, out int UnitCount)
	{
		if (m_aGroups.Count() == 0)
		{
			return null;
		}
		int USSRcount = 0;
	    int UScount = 0;
	    int FIAcount = 0;
	    int Banditcount = 0;
	    int Renegcount = 0;
	
	    FactionManager FMan = FactionManager.Cast(GetGame().GetFactionManager());
	    Faction MajorFaction = null;
	    int max = 0;
	    for (int i = m_aGroups.Count() - 1; i >= 0; i--)
	    {
			if(m_aGroups[i] == null)
			{
				return null;
			}
	        string faction = m_aGroups[i].GetFaction().GetFactionKey();
	        int agentsCount = m_aGroups[i].GetAgentsCount();
	
	        switch (faction)
	        {
	            case "USSR":
	                USSRcount += agentsCount;
	                if (USSRcount > max)
	                {
	                    max = USSRcount;
	                    MajorFaction = FMan.GetFactionByKey("USSR");
	                    factionReadable = "soviet";
	                }
	                break;
	            case "US":
	                UScount += agentsCount;
	                if (UScount > max)
	                {
	                    max = UScount;
	                    MajorFaction = FMan.GetFactionByKey("US");
	                    factionReadable = "US";
	                }
	                break;
	            case "FIA":
	                FIAcount += agentsCount;
	                if (FIAcount > max)
	                {
	                    max = FIAcount;
	                    MajorFaction = FMan.GetFactionByKey("FIA");
	                    factionReadable = "guerrilla";
	                }
	                break;
	            case "BANDITS":
	                Banditcount += agentsCount;
	                if (Banditcount > max)
	                {
	                    max = Banditcount;
	                    MajorFaction = FMan.GetFactionByKey("BANDITS");
	                    factionReadable = "bandit";
	                }
	                break;
	            case "RENEGADE":
	                Renegcount += agentsCount;
	                if (Renegcount > max)
	                {
	                    max = Renegcount;
	                    MajorFaction = FMan.GetFactionByKey("RENEGADE");
	                    factionReadable = "renegade";
	                }
	                break;
	        }
	    }
	
		UnitCount = max;
		if(!MajorFaction)
			return null;
	    return MajorFaction; 	
	}
	Faction GetMajorityHolder(out string factionReadable)
	{
		if (m_aGroups.Count() == 0)
		{
			return null;
		}
	    int USSRcount = 0;
	    int UScount = 0;
	    int FIAcount = 0;
	    int Banditcount = 0;
	    int Renegcount = 0;
	
	    FactionManager FMan = FactionManager.Cast(GetGame().GetFactionManager());
	    Faction MajorFaction = null;
	    int max = 0;
	
	    for (int i = m_aGroups.Count() - 1; i >= 0; i--)
	    {
			if(m_aGroups[i] == null)
			{
				return null;
			}
	        string faction = m_aGroups[i].GetFaction().GetFactionKey();
	        int agentsCount = m_aGroups[i].GetAgentsCount();
	
	        switch (faction)
	        {
	            case "USSR":
	                USSRcount += agentsCount;
	                if (USSRcount > max)
	                {
	                    max = USSRcount;
	                    MajorFaction = FMan.GetFactionByKey("USSR");
	                    factionReadable = "soviet";
	                }
	                break;
	            case "US":
	                UScount += agentsCount;
	                if (UScount > max)
	                {
	                    max = UScount;
	                    MajorFaction = FMan.GetFactionByKey("US");
	                    factionReadable = "US";
	                }
	                break;
	            case "FIA":
	                FIAcount += agentsCount;
	                if (FIAcount > max)
	                {
	                    max = FIAcount;
	                    MajorFaction = FMan.GetFactionByKey("FIA");
	                    factionReadable = "guerrilla";
	                }
	                break;
	            case "BANDITS":
	                Banditcount += agentsCount;
	                if (Banditcount > max)
	                {
	                    max = Banditcount;
	                    MajorFaction = FMan.GetFactionByKey("BANDITS");
	                    factionReadable = "bandit";
	                }
	                break;
	            case "RENEGADE":
	                Renegcount += agentsCount;
	                if (Renegcount > max)
	                {
	                    max = Renegcount;
	                    MajorFaction = FMan.GetFactionByKey("RENEGADE");
	                    factionReadable = "renegade";
	                }
	                break;
	        }
	    }
	
	    return MajorFaction;
	}*/
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
		if(m_aQueriedSentinels)
			m_aQueriedSentinels.Clear();
		if(m_aQueriedPrefabSpawnP)
			m_aQueriedPrefabSpawnP.Clear();
		if(m_aGroups)
			m_aGroups.Clear();
	}	
	
	/*string GetCharacterLocation(IEntity Character)
	{
		vector mins,maxs;
		GetGame().GetWorldEntity().GetWorldBounds(mins, maxs);
			
		m_iGridSizeX = maxs[0]/3;
		m_iGridSizeY = maxs[2]/3;
	 
		SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
		vector posPlayer = Character.GetOrigin();
			
		SCR_EditableEntityComponent nearest = core.FindNearestEntity(posPlayer, EEditableEntityType.COMMENT);
		GenericEntity nearestLocation = nearest.GetOwner();
		SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(nearestLocation.FindComponent(SCR_MapDescriptorComponent));
		string closestLocationName;
		closestLocationName = nearest.GetDisplayName();

		vector lastLocationPos = nearestLocation.GetOrigin();
		float lastDistance = vector.DistanceSqXZ(lastLocationPos, posPlayer);
	
		string closeLocationAzimuth;
		vector result = posPlayer - lastLocationPos;
		result.Normalize();
		
		float angle1 = vector.DotXZ(result,vector.Forward);
		float angle2 = vector.DotXZ(result,vector.Right);
				
		if (angle2 > 0)
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthEast";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionEast";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthEast";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		else
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthWest";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionWest";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthWest";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		int playerGridPositionX = posPlayer[0]/m_iGridSizeX;
		int playerGridPositionY = posPlayer[2]/m_iGridSizeY;
			
		int playerGridID = GetGridIndex(playerGridPositionX,playerGridPositionY);
	 	string m_sLocationName = m_WorldDirections.GetQuadHint(playerGridID) + ", " + closestLocationName;
		return m_sLocationName;
	}*/
	void Init()
	{
		if(!GetGame().GetWorldEntity())
		{
			return;
		}
		if(!m_aQueriedSentinels)
			m_aQueriedSentinels = new ref array<IEntity>();
		_CaptureSentinels();
		if(!AllDirectors)
			AllDirectors = new ref array<SP_AIDirector>();
		
		AllDirectors.Insert(this);
		
		m_fRespawnPeriod = Math.RandomFloat(1, 20);
		// get first children, it should be WP
		//vector position = GetOrigin();
		//vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		//spawnMatrix[3] = position;
		//EntitySpawnParams WPspawnParams = EntitySpawnParams();
		//WPspawnParams.TransformMode = ETransformMode.WORLD;
		//WPspawnParams.Transform = spawnMatrix;
		//Resource WP = Resource.Load(m_pDefaultWaypoint);
		//DefWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(WP, null, WPspawnParams));
		//DefWaypoint.SetCompletionRadius(m_fRadius);
		GetGame().GetCallqueue().CallLater(SpawnPrefab, 2, false);
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
		if(!m_pCharToSpawn)
		{
			SetCharToSpawn();
		}
		//if(m_vPositiontoSpawn == vector.Zero)
		//{
		//	SetSpawnPos();
		//}
		if (m_bRespawn)
		{
			if (m_aGroups.Count() < m_iMaxAgentsToSpawn)
			{
				if (m_fRespawnPeriod <= 0.0)
				{
					IEntity entity = slot.GetOwner();
					if (!m_FactionsToApear.IsEmpty())
						slot.SetFactionKey(m_FactionsToApear.GetRandomElement());
					slot.m_eEntityCatalogType = m_eEntityCatalogType;
					slot.m_aIncludedEditableEntityLabels.Clear();
					slot.m_aIncludedEditableEntityLabels.Copy(m_aIncludedEditableEntityLabels);
					slot.m_aExcludedEditableEntityLabels.Clear();
					slot.m_aExcludedEditableEntityLabels.Copy(m_aExcludedEditableEntityLabels);
					if (!m_aQueriedSentinels.IsEmpty())
						entity.SetOrigin(m_aQueriedSentinels.GetRandomElement().GetOrigin());
					slot.SelectRandomSlot();
					slot.SpawnAsset();
					slot.InitEnt();
					slot.SetWPGroup();
					OnSpawn(slot.GetSpawnedEntity());
				}
				else
				{
					m_fRespawnPeriod -= m_iDirectorUpdatePeriod;
				}	
			}
		}
		super.EOnFrame(owner, timeSlice);
	}
	bool Spawn()
	{	
		if(!m_pCharToSpawn)
		{
			return false;
		}
		if (m_aQueriedSentinels.IsEmpty())
			_CaptureSentinels();
		foreach (IEntity sentinel : m_aQueriedSentinels)
			{
				SCR_AISmartActionSentinelComponent sent = SCR_AISmartActionSentinelComponent.Cast(sentinel.FindComponent(SCR_AISmartActionSentinelComponent));
				m_vPositiontoSpawn = sentinel.GetOrigin() + sent.GetActionOffset();
				m_aQueriedSentinels.RemoveItem(sentinel);
				break;
			}
		if (m_vPositiontoSpawn == vector.Zero)
			return false;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = m_vPositiontoSpawn;	
		Resource res;
		IEntity newEnt;
		res = Resource.Load(m_pCharToSpawn);
		
		newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		if (!newEnt)
			return false;
		if (newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
		
		m_vPositiontoSpawn = vector.Zero;
		m_pCharToSpawn = STRING_EMPTY;
		return true;
	}
	void ScatterAI(AIAgent child)
	{
		if (m_aQueriedSentinels.IsEmpty())
				_CaptureSentinels();
		
		if (!child)
			return;
		IEntity ent = child.GetControlledEntity();
		if (!ent)
			return;
		SCR_AISmartActionSentinelComponent sent = SCR_AISmartActionSentinelComponent.Cast(ent.FindComponent(SCR_AISmartActionSentinelComponent));
		SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(ent.FindComponent(SCR_EditableEntityComponent));
		vector mat[4];
		ent.GetWorldTransform(mat);
		mat[3] = ent.GetOrigin() + sent.GetActionOffset();
		if (mat != vector.Zero)
			editable.SetTransform(mat, false);
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
	void SetSpawnPos()
	{
		if (m_aQueriedSentinels.IsEmpty())
			_CaptureSentinels();
		if (!m_aQueriedSentinels.IsEmpty())
		{
			Math.Randomize(-1);
			IEntity ent = m_aQueriedSentinels[Math.RandomInt(0, m_aQueriedSentinels.Count())];
			if (m_bAllowCloseSpawning)
			{
				SCR_AISmartActionSentinelComponent sent = SCR_AISmartActionSentinelComponent.Cast(ent.FindComponent(SCR_AISmartActionSentinelComponent));
				m_vPositiontoSpawn = ent.GetOrigin() + sent.GetActionOffset();
				m_aQueriedSentinels.RemoveItem(ent);
				return;
			}
			else
			{
				if (!CheckForCharacters(2, ent.GetOrigin()) && m_aQueriedSentinels.Count() > 0)
				{
					return;
				}
				SCR_AISmartActionSentinelComponent sent = SCR_AISmartActionSentinelComponent.Cast(ent.FindComponent(SCR_AISmartActionSentinelComponent));
				m_vPositiontoSpawn = ent.GetOrigin() + sent.GetActionOffset();
				m_aQueriedSentinels.RemoveItem(ent);
				return;
			}
		}
		/*
		// randomize position in radius
		vector position = GetOrigin();
		float yOcean = GetWorld().GetOceanBaseHeight();
		RandomGenerator rand = new RandomGenerator();
		position[1] = yOcean - 1; // force at least one iteration
		position[0] = position[0] + rand.RandFloatXY(-m_fRadius, m_fRadius);
		position[2] = position[2] + rand.RandFloatXY(-m_fRadius, m_fRadius);
		position[1] = GetWorld().GetSurfaceY(position[0], position[2]);
		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = spawnMatrix;
		m_vPositiontoSpawn = spawnParams.Transform[3];
		float surfaceY = GetGame().GetWorld().GetSurfaceY(m_vPositiontoSpawn[0], m_vPositiontoSpawn[2]);
		if (m_vPositiontoSpawn[1] < surfaceY)
		{
			m_vPositiontoSpawn[1] = surfaceY;
		}
		//Snap to the nearest navmesh point
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(m_vPositiontoSpawn, "50 50 50", m_vPositiontoSpawn))
		{
			float groundHeight = GetGame().GetWorld().GetSurfaceY(m_vPositiontoSpawn[0], m_vPositiontoSpawn[2]);
			if (m_vPositiontoSpawn[1] < groundHeight)
				m_vPositiontoSpawn[1] = groundHeight;
		}*/
	}
	bool CreateVictim(out IEntity Victim)
	{
		if (!m_bAllowRescue)
		{
			return false;
		}
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		array<AIAgent> outAgents = new array<AIAgent>();
		SCR_AIGroup luckygroup = m_aGroups.GetRandomElement();
		
		if(!luckygroup)
		{
			return false;
		}
		luckygroup.GetAgents(outAgents);
		if(outAgents.Count() <= 0)
		{
			return false;
		}
		
		Victim = luckygroup.GetLeaderEntity();
		if(!Victim)
		{
			return false;
		}
		foreach(AIAgent agent : outAgents)
		{
			IEntity Char = agent.GetControlledEntity();
			if(Char)
			{
				SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
				if(dmg.GetIsUnconscious())
				{
					return false;
				}
				dmg.ForceUnconsciousness();
				dmg.SetPermitUnconsciousness(false, true);
				dmg.AddRandomBleeding();
			}
		}
		return true;
	};
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
	/*override void _WB_SetExtraVisualiser(EntityVisualizerType type, IEntitySource src)
	{		
		m_bVisualize = false;
		switch (type)
		{
			case EntityVisualizerType.EVT_NONE:
				return;
			
			case EntityVisualizerType.EVT_NORMAL:
				return;
		}
		
		m_bVisualize = true;		
		super._WB_SetExtraVisualiser(type, src);
	}
	*/
	private bool QueryEntitiesForSentinels(IEntity e)
	{
		SCR_AISmartActionSentinelComponent sentinel = SCR_AISmartActionSentinelComponent.Cast(e.FindComponent(SCR_AISmartActionSentinelComponent));
		if (sentinel)
			{
				m_aQueriedSentinels.Insert(e);
			}
		return true;
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

	private void GetSentinels(float radius)
	{
		BaseWorld world = GetWorld();
		world.QueryEntitiesBySphere(GetOrigin(), radius, QueryEntitiesForSentinels);
	}
	private void GetPrefabSpawns(float radius)
	{
		BaseWorld world = GetWorld();
		world.QueryEntitiesBySphere(GetOrigin(), radius, QueryEntitiesForPrefabSpawner);
	}
	private void _CaptureSentinels()
	{
		m_aQueriedSentinels = new array <IEntity>();
		GetSentinels(m_fRadius);
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
			return;
		}
		FactionManager factionManager = GetGame().GetFactionManager();
		if(!factionManager)
		{
			return;
		}
		foreach(IEntity prefabspawner : m_aQueriedPrefabSpawnP)
		{
			SCR_PrefabSpawnPoint Pspawn = SCR_PrefabSpawnPoint.Cast(prefabspawner);
			ResourceName prefab;
			if(Pspawn.GetType() == EPrefabSpawnType.MilitaryVehicles)
			{
				SCR_Faction randfaction = SCR_Faction.Cast(factionManager.GetFactionByKey(m_FactionsToApear.GetRandomElement()));
				SCR_EntityCatalog entityCatalog = randfaction.GetFactionEntityCatalogOfType(EEntityCatalogType.VEHICLE);
				array<SCR_EntityCatalogEntry> aFactionEntityEntry = new array<SCR_EntityCatalogEntry>();
				entityCatalog.GetEntityList(aFactionEntityEntry);
				prefab = aFactionEntityEntry.GetRandomElement().GetPrefab();
			}
			if(Pspawn.GetType() == EPrefabSpawnType.Generic)
			{
				SCR_EntityCatalogManagerComponent CatalogM = SCR_EntityCatalogManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_EntityCatalogManagerComponent));
				SCR_EntityCatalog entityCatalog = CatalogM.GetEntityCatalogOfType(EEntityCatalogType.STASH);
				array<SCR_EntityCatalogEntry> aEntityEntry = new array<SCR_EntityCatalogEntry>();
				entityCatalog.GetEntityList(aEntityEntry);
				prefab = aEntityEntry.GetRandomElement().GetPrefab();
			}
			EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
			vector spawnpos[4];
			Pspawn.GetWorldTransform(spawnpos);
			Resource prefabtospawn = Resource.Load(prefab);
			SCR_TerrainHelper.SnapAndOrientToTerrain(spawnpos, GetWorld());
			PrefabspawnParams.Transform = spawnpos;
			IEntity Entity = GetGame().SpawnEntityPrefab(prefabtospawn, null, PrefabspawnParams);
			SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
			if(aiWorld)
				aiWorld.RequestNavmeshRebuildEntity(Entity);
			delete prefabspawner;
		}
		
		m_aQueriedPrefabSpawnP.Clear();
	}
	override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent) 
	{
		_CaptureSentinels();
		_CapturePrefabSpawns();
		return super._WB_OnKeyChanged(src, key, ownerContainers, parent);
	}
	
	override void _WB_SetExtraVisualiser(EntityVisualizerType type, IEntitySource src)
	{	
		/*m_bVisualize = false;
		switch (type)
		{
			case EntityVisualizerType.EVT_NONE:
				return;
			
			case EntityVisualizerType.EVT_NORMAL:
				return;
		}
		
		m_bVisualize = true;*/
		_CaptureSentinels();
		_CapturePrefabSpawns();
		super._WB_SetExtraVisualiser(type, src);
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
			if(m_aQueriedSentinels && m_bVisualize)
			{
				foreach (IEntity entity : m_aQueriedSentinels)
				{
					vector entorigin = entity.GetOrigin();
					Shape.CreateSphere(Color.PINK, ShapeFlags.WIREFRAME | ShapeFlags.ONCE, entorigin, 5);
					array<Managed> outComponents = new array<Managed>();
					entity.FindComponents(SCR_AISmartActionSentinelComponent, outComponents);
					foreach(Managed smart : outComponents)
					{
						SCR_AISmartActionSentinelComponent sent = SCR_AISmartActionSentinelComponent.Cast(smart);
						array<string> outTagstemp = new array<string>();
						sent.GetTags(outTagstemp);
						string tags;
						foreach(string tag : outTagstemp)
						{
							tags = tags +  "|" + tag + "|\n";
						}
						vector Smartloc = entorigin + sent.GetActionOffset();
						Shape.CreateSphere(Color.PINK, ShapeFlags.DEFAULT | ShapeFlags.ONCE, Smartloc, 1);
						DebugTextWorldSpace.Create(GetGame().GetWorld(), tags, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, Smartloc[0], Smartloc[1] + 1, Smartloc[2], 5, 0xFFFFFFFF, Color.BLACK);
						tags = STRING_EMPTY;
					}
					string SmartText = string.Format("%1: seats", outComponents.Count().ToString());
					DebugTextWorldSpace.Create(GetGame().GetWorld(), SmartText, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, entorigin[0], entorigin[1] + 5, entorigin[2], 10, 0xFFFFFFFF, Color.BLACK);
					
				}
			}
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