modded class SCR_CampaignMilitaryBaseMapDescriptorComponent
{
	//------------------------------------------------------------------------------------------------
	override void MapSetup(notnull Faction faction)
	{
		return;
		MapItem item = Item();
		Faction baseFaction = m_Base.GetFaction();
		
		if (m_Base.IsHQ() && faction != baseFaction)
		{
			item.SetVisible(false);
			return;
		}
		else
		{
			item.SetVisible(true);
			item.SetDisplayName(m_Base.GetBaseName());
			
			MapDescriptorProps props = item.GetProps();
			props.SetDetail(96);
			
			Color rangeColor;
			
			if (baseFaction)
				rangeColor = baseFaction.GetFactionColor();
			else
				rangeColor = Color(1, 1, 1, 1);
			
			props.SetOutlineColor(rangeColor);
			rangeColor.SetA(0.1);
			props.SetBackgroundColor(rangeColor);
			
			item.SetProps(props);
			item.SetRange(0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleMapLinks(bool unlinkHQ = false)
	{
		return;
		if (m_Base.IsHQ() && m_Base.GetFaction() != SCR_FactionManager.SGetLocalPlayerFaction())
			return;
		
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		
		if (!campaign)
			return;

		UnregisterMyMapLinks(unlinkHQ);
		SCR_CampaignFaction localPlayerFaction = SCR_CampaignFaction.Cast(SCR_FactionManager.SGetLocalPlayerFaction());
		
		if (!localPlayerFaction)
			return;
		
		SCR_CampaignMobileAssemblyStandaloneComponent mobileHQ = localPlayerFaction.GetMobileAssembly();
		MapItem mobilehqMapItem;
		
		if (mobileHQ)
		{
			SCR_SpawnPoint spawnpoint = SCR_SpawnPoint.Cast(mobileHQ.GetOwner());
			
			if (spawnpoint)
			{
				SCR_MapDescriptorComponent desc = SCR_MapDescriptorComponent.Cast(spawnpoint.FindComponent(SCR_MapDescriptorComponent));
				
				if (desc && mobileHQ.IsInRadioRange())
				{
					array<SCR_CampaignMilitaryBaseComponent> basesInRangeOfMobileHQ = {};
					mobileHQ.GetBasesInRange(basesInRangeOfMobileHQ);
					
					if (basesInRangeOfMobileHQ.Contains(m_Base))
						mobilehqMapItem = desc.Item();
				}
			}
		}
		
		SCR_CampaignFaction faction = m_Base.GetCampaignFaction();
		
		if (!mobilehqMapItem)
		{
			if (!m_Base.IsHQRadioTrafficPossible(localPlayerFaction) || localPlayerFaction != faction)
				return;
		}
		
		CreateLinks(faction, localPlayerFaction, mobilehqMapItem);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Shows info about the base in the map
	override void HandleMapInfo(SCR_CampaignFaction playerFactionCampaign = null)
	{
		return;
		SCR_GameModeCampaign campaignGameMode = SCR_GameModeCampaign.GetInstance();

		if (!campaignGameMode)
			return;
		
		if (!playerFactionCampaign)
			playerFactionCampaign = SCR_CampaignFaction.Cast(SCR_FactionManager.SGetLocalPlayerFaction());
		
		if (!playerFactionCampaign)
			playerFactionCampaign = campaignGameMode.GetBaseManager().GetLocalPlayerFaction();

		if (!playerFactionCampaign)
			return;

		if (m_Base.IsHQ() && m_Base.GetFaction() != playerFactionCampaign)
			return;
		
		// Set callsign based on player's faction
		if (m_Base.GetType() != SCR_ECampaignBaseType.RELAY && m_Base.GetCallsignDisplayName().IsEmpty())
			m_Base.SetCallsign(playerFactionCampaign);
		
		SCR_CampaignMapUIBase mapUI = m_Base.GetMapUI();
		
		if (mapUI)
			mapUI.SetIconInfoText();

		// Update base icon color
		EFactionMapID factionMapID = EFactionMapID.UNKNOWN;
		bool isInRange = m_Base.IsHQRadioTrafficPossible(playerFactionCampaign);

		// Show proper faction color only for HQs or bases within radio signal
		if (m_Base.GetFaction() && (m_Base.IsHQ() || isInRange))
		{
			switch (m_Base.GetFaction().GetFactionKey())
			{
				case campaignGameMode.GetFactionKeyByEnum(SCR_ECampaignFaction.OPFOR): {factionMapID = EFactionMapID.EAST; break;};
				case campaignGameMode.GetFactionKeyByEnum(SCR_ECampaignFaction.BLUFOR): {factionMapID = EFactionMapID.WEST; break;};
				case campaignGameMode.GetFactionKeyByEnum(SCR_ECampaignFaction.INDFOR): {factionMapID = EFactionMapID.FIA; break;};
			}
		}
		
		Item().SetFactionIndex(factionMapID);

		array<SCR_ServicePointDelegateComponent> delegates = {};
		m_Base.GetServiceDelegates(delegates);
		
		foreach (SCR_ServicePointDelegateComponent delegate: delegates)
		{
			IEntity owner = delegate.GetOwner();
			
			if (!owner)
				continue;
			
			SCR_ServicePointMapDescriptorComponent comp = SCR_ServicePointMapDescriptorComponent.Cast(owner.FindComponent(SCR_ServicePointMapDescriptorComponent));

			if (comp)
			{
				if (isInRange)
					comp.SetServiceMarker(m_Base.GetCampaignFaction());
				else
					comp.SetServiceMarker(visible: false);
			}
		}

		if (mapUI)
			mapUI.UpdateBaseIcon(factionMapID);
		
		HandleMapLinks();
	}
};