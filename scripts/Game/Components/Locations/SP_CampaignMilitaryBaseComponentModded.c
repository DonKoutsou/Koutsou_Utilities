modded class SCR_MapCampaignUI
{
	override protected void InitBases()
	{
		SCR_MilitaryBaseManager baseManager = SCR_MilitaryBaseManager.GetInstance();
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		
		if (!campaign)
			return;
		
		SCR_CampaignMilitaryBaseManager CbaseManager = campaign.GetBaseManager();
		array<SCR_MilitaryBaseComponent> bases = {};
		baseManager.GetBases(bases);
		SCR_CampaignFaction faction = SCR_CampaignFaction.Cast(SCR_FactionManager.SGetLocalPlayerFaction());
		
		if (!faction)
			return;
		
		SCR_CampaignMilitaryBaseComponent base;
		
		for (int i = 0, count = bases.Count(); i < count; ++i)
		{
			base = SCR_CampaignMilitaryBaseComponent.Cast(bases[i]);
			
			if (!base || !base.IsInitialized() || !CbaseManager.IsBaseVisible(base))
				continue;
			
			if (base.IsHQ() && base.GetFaction().IsFactionEnemy(faction))
				continue;

			Widget w = GetGame().GetWorkspace().CreateWidgets(m_sBaseElement, m_wIconsContainer);
			SCR_CampaignMapUIBase handler = SCR_CampaignMapUIBase.Cast(w.FindHandler(SCR_CampaignMapUIBase));
			
			if (!handler)
				return;

			handler.SetParent(this);
			handler.InitBase(base);
			m_mIcons.Set(w, handler);
			base.SetBaseUI(handler);

			FrameSlot.SetSizeToContent(w, true);
			FrameSlot.SetAlignment(w, 0.5, 0.5);
		}

		if (faction)
		{
			string factionKey = faction.GetFactionKey();
			InitMobileAssembly(factionKey, faction.GetMobileAssembly() != null);
		}
		
		UpdateIcons();
	}
	
	/*override void OnMapOpen(MapConfiguration config)
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (pc)
		{
			m_PlyFactionAffilComp = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
			if (m_PlyFactionAffilComp)
				m_PlyFactionAffilComp.GetOnPlayerFactionResponseInvoker_O().Insert(OnPlayerFactionResponse);	

			m_PlyGroupComp = SCR_PlayerControllerGroupComponent.Cast(pc.FindComponent(SCR_PlayerControllerGroupComponent));
			if (m_PlyGroupComp)
				m_PlyGroupComp.GetOnGroupChanged().Insert(OnPlayerGroupChanged);		
		}
		
		SCR_SpawnPoint.Event_SpawnPointFactionAssigned.Insert(OnSpawnPointFactionChange);
		SCR_SpawnPoint.Event_SpawnPointAdded.Insert(AddSpawnPoint);
		SCR_SpawnPoint.Event_SpawnPointRemoved.Insert(RemoveSpawnPoint);

		SCR_BaseTaskManager.s_OnTaskUpdate.Insert(OnTaskAdded);

		m_bIsDeployMap = (config.MapEntityMode == EMapEntityMode.SPAWNSCREEN);

		m_wIconsContainer = m_RootWidget.FindAnyWidget(m_sIconsContainer);
		m_wIconsContainer.SetVisible(true);

		Widget child = m_wIconsContainer.GetChildren();
		while (child)
		{
			Widget sibling = child.GetSibling();
			delete child;
			child = sibling;
		}

		if (m_bShowSpawnPoints)
			InitSpawnPoints();

		if (m_bShowTasks)
			InitTaskMarkers();
		
		m_MapEntity.GetOnMapPan().Insert(OnMapPan);
	}*/
}
