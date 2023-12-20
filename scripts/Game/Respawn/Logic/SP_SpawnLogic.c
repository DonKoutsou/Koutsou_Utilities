[BaseContainerProps(category: "Respawn")]
modded class SCR_AutoSpawnLogic
{
	[Attribute("1")]
	int m_iLives;
	
	[Attribute("Erquy FIA Outpost")]
	string m_sFactionBaseToSpawnAt;
	
	[Attribute("{A1CE9D1EC16DA9BE}UI/layouts/Menus/MainMenu/SplashScreen.layout", desc: "Layout shown before deploy menu opens on client")]
	protected ResourceName m_sLoadingLayout;
	
	[Attribute("40", UIWidgets.Slider, params: "0 1000 1")]
	protected int m_iPlayerInitialRep;
	
	
	bool m_bAllowTaskMarkers;
	protected Widget m_wLoadingPlaceholder;
	protected ButtonWidget m_wCloseButton;
	bool m_bDissableMetabolism;
	ref ScriptInvoker m_OnMetDiss = new ScriptInvoker();
	ref ScriptInvoker m_OnMetEna = new ScriptInvoker();
	
	void OnMetDissabled()
	{
		m_OnMetDiss.Invoke();
	}
	void OnMetEnabled()
	{
		m_OnMetEna.Invoke();
	}
	void SetLives(int amount)
	{
		m_iLives = amount;
	}
	void SetTaskMarkers(bool toset)
	{
		m_bAllowTaskMarkers = toset;
	}
	void SetName(string name, string surname)
	{
		if (!name && !surname)
			return;
		
		CharacterIdentityComponent id = CharacterIdentityComponent.Cast(GetGame().GetPlayerController().GetControlledEntity().FindComponent(CharacterIdentityComponent));
		
		if (name)
			id.GetIdentity().SetName(name);
		if (surname)
			id.GetIdentity().SetSurname(surname);
	}
	void EnablePost()
	{
		GetGame().GetWorld().SetCameraPostProcessEffect(0, 15, PostProcessEffectType.HDR, "{9E4227906BE39169}Common/Postprocess/HDR_Effecty.emat");
		GetGame().GetWorld().SetCameraPostProcessEffect(0, 16, PostProcessEffectType.FilmGrain, "{0640A45321D6D9B9}Common/Postprocess/HDR_EffectyFGrain.emat");
	}
	//------------------------------------------------------------------------------------------------
	override void OnInit(SCR_RespawnSystemComponent owner)
	{
		if (!System.IsConsoleApp())
			CreateLoadingPlaceholder();
		super.OnInit(owner);
	}
	override void OnPlayerKilled_S(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled_S(playerId, playerEntity, killerEntity, killer);
		m_iLives -= 1;
		if (m_iLives == 0)
		{
			SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
			if (!manager)
				return;
			manager.SetMissionEndScreen(EGameOverTypes.DEATH);
			manager.Finish();
			return;
		}
		
	}
	override void OnPlayerSpawned_S( int playerId, IEntity entity)
	{
		super.OnPlayerSpawned_S(playerId, entity);
		//set initial rep
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(entity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(m_iPlayerInitialRep);
	}
	override protected void Spawn(int playerId)
	{
		// Player is disconnecting (and disappearance of controlled entity started this feedback loop).
		// Simply ignore such requests as it would create unwanted entities.
		int indexOf = m_DisconnectingPlayers.Find(playerId);
		if (indexOf != -1)
		{
			m_DisconnectingPlayers.Remove(indexOf);
			return;
		}
		
		array<Faction> factions = {};
		GetGame().GetFactionManager().GetFactionsList(factions);		
		
		Faction targetFaction;
		if (!GetForcedFaction(targetFaction))
			targetFaction = factions.GetRandomElement();
		
		// Request both
		if (!GetPlayerFactionComponent_S(playerId).RequestFaction(targetFaction))
		{
			// Try again later
		}

		SCR_BasePlayerLoadout targetLoadout;
		if (!GetForcedLoadout(targetLoadout))
			targetLoadout = GetGame().GetLoadoutManager().GetRandomFactionLoadout(targetFaction);		
		
		if (!GetPlayerLoadoutComponent_S(playerId).RequestLoadout(targetLoadout))
		{
			// Try again later
		}

		Faction faction = GetPlayerFactionComponent_S(playerId).GetAffiliatedFaction();
		if (!faction)
		{
			OnPlayerSpawnFailed_S(playerId);
			return;
		}

		SCR_BasePlayerLoadout loadout = GetPlayerLoadoutComponent_S(playerId).GetLoadout();
		if (!loadout)
		{
			OnPlayerSpawnFailed_S(playerId);
			return;
		}
		SCR_CampaignMilitaryBaseManager campman = SCR_CampaignMilitaryBaseManager.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager());
		SCR_SpawnPoint point = campman.GetNamedBase(m_sFactionBaseToSpawnAt).GetSpawnPoint();
		if (!point)
		{
			OnPlayerSpawnFailed_S(playerId);
			return;
		}

		SCR_SpawnPointSpawnData data = new SCR_SpawnPointSpawnData(loadout.GetLoadoutResource(), point.GetRplId());
		if (GetPlayerRespawnComponent_S(playerId).CanSpawn(data))
			DoSpawn(playerId, data);
		else
			OnPlayerSpawnFailed_S(playerId);
		
		
	}
	//------------------------------------------------------------------------------------------------
	protected void CreateLoadingPlaceholder()
	{
		m_wLoadingPlaceholder = GetGame().GetWorkspace().CreateWidgets(m_sLoadingLayout);
		if (!m_wLoadingPlaceholder)
			return;
		
	}
	//------------------------------------------------------------------------------------------------
	void DestroyLoadingPlaceholder()
	{
		if (!m_wLoadingPlaceholder)
			return;

		m_wLoadingPlaceholder.RemoveFromHierarchy();
		m_wLoadingPlaceholder = null;
		GetGame().GetMenuManager().CloseAllMenus();
	}
	void OpenStartingScreenUI()
	{
		MenuManager menuManager = g_Game.GetMenuManager();
		MenuBase myMenu = menuManager.OpenMenu(ChimeraMenuPreset.StartingScreenMenu);
		GetGame().GetInputManager().ActivateContext("StartingScreenContext");
		SP_StartingScreenUI SSUI = SP_StartingScreenUI.Cast(myMenu);
		SSUI.Init();

	}
};
