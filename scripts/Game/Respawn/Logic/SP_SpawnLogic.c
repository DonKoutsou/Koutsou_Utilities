[BaseContainerProps(category: "Respawn")]
modded class SCR_AutoSpawnLogic
{
	[Attribute("1")]
	int m_iLives;
	
	[Attribute("FIA")]
	string m_sFactionBaseToSpawnAt;
	
	[Attribute("{A1CE9D1EC16DA9BE}UI/layouts/Menus/MainMenu/SplashScreen.layout", desc: "Layout shown before deploy menu opens on client")]
	protected ResourceName m_sLoadingLayout;

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

		SCR_SpawnPoint point = SCR_SpawnPoint.GetRandomSpawnPointForFaction(m_sFactionBaseToSpawnAt);
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
		//m_wLoadingPlaceholder = GetGame().GetWorkspace().CreateWidgets(m_sLoadingLayout);
		//if (!m_wLoadingPlaceholder)
			//return;
		
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
