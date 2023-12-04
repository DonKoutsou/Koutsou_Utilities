modded class SCR_CampaignFeedbackComponent
{
	
	//------------------------------------------------------------------------------------------------
	override protected void CheckPlayerInsideRadioRange()
	{
		SCR_CampaignFactionManager fManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());

		if (!fManager)
			return;

		IEntity player = SCR_PlayerController.GetLocalControlledEntity();

		if (!player)
			return;

		SCR_CampaignFaction faction = SCR_CampaignFaction.Cast(SCR_FactionManager.SGetLocalPlayerFaction());

		if (!faction)
			return;

		bool isInRangeNow = m_Campaign.GetBaseManager().IsEntityInFactionRadioSignal(player, faction);

		if (isInRangeNow != m_bIsPlayerInRadioRange)
		{
			m_bIsPlayerInRadioRange = isInRangeNow;

			//if (isInRangeNow)
			//{
				//SCR_PopUpNotification.GetInstance().PopupMsg("#AR-Campaign_RadioRangeEntered-UC", duration: 3);
				//SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_RADIO_SIGNAL_GAIN);
			//}
			//else
			//{
				//SCR_PopUpNotification.GetInstance().PopupMsg("#AR-Campaign_RadioRangeLeft-UC", duration: 3);
				//SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_RADIO_SIGNAL_LOST);
			//}
		}
	}
	//------------------------------------------------------------------------------------------------
	override void OnBaseEntered(notnull SCR_CampaignMilitaryBaseComponent base)
	{
		if (m_BaseWithPlayer)
			OnBaseLeft(m_BaseWithPlayer);

		m_BaseWithPlayer = base;
		
		PlayerController playerController = GetGame().GetPlayerController();
		
		
		if (playerController)
		{
			SCR_CampaignNetworkComponent campaignNetworkComponent = SCR_CampaignNetworkComponent.Cast(playerController.FindComponent(SCR_CampaignNetworkComponent));

			if (campaignNetworkComponent)
				campaignNetworkComponent.RespawnLocationPopup(base.GetCallsign());
			
			SCR_GameModeCampaign GameMode = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
			SCR_CampaignMilitaryBaseManager BaseMan = GameMode.GetBaseManager();
			if ( ! BaseMan.IsBaseVisible(base) )
			{
				BaseMan.SetBaseVisible(base);
			}
		}
		FactionAffiliationComponent factionaff = FactionAffiliationComponent.Cast(playerController.GetControlledEntity().FindComponent(FactionAffiliationComponent));
		if ( base.GetFaction().IsFactionEnemy (factionaff.GetAffiliatedFaction()))
		{
			return;
		}
		SP_RequestManagerComponent Rman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		Rman.CreateChainedBaseTask(base.GetBaseName());
		
		if (!base.m_bVisited)
		{
				SCR_AIGroup group = base.GetDefendersGroup();
			if (!group)
			{
				array <IEntity> BaseGuards = {};
				base.GetGuards(BaseGuards);
				if (!BaseGuards.IsEmpty())
				{
					group = SCR_AIGroup.Cast( ChimeraCharacter.Cast( BaseGuards.GetRandomElement() ).GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup() );
					
				}
			}
			if (group)
			{
				SendGuardToPlayer(group);
			}
		}
		RecordBaseEnteredEvent(base, playerController.GetControlledEntity());
		/*
		SCR_CampaignSeizingComponent seizingComponent = SCR_CampaignSeizingComponent.Cast(m_BaseWithPlayer.GetOwner().FindComponent(SCR_CampaignSeizingComponent));

		if (seizingComponent)
		{
			OnSeizingTimerChange(seizingComponent.GetSeizingStartTimestamp(), seizingComponent.GetSeizingEndTimestamp());
			seizingComponent.GetOnTimerChange().Remove(OnSeizingTimerChange);
			seizingComponent.GetOnTimerChange().Insert(OnSeizingTimerChange);
		}
		else
		{
			#ifndef AR_CAMPAIGN_TIMESTAMP
			OnSeizingTimerChange(0, 0);
			#else
			OnSeizingTimerChange(null, null);
			#endif
		}
		SCR_CampaignFaction playerFaction = SCR_CampaignFaction.Cast(SCR_FactionManager.SGetLocalPlayerFaction());

		if (m_BaseWithPlayer.GetFaction() != playerFaction)
		{
			if (m_BaseWithPlayer.IsHQRadioTrafficPossible(playerFaction))
			{
				if (m_BaseWithPlayer.GetType() == SCR_ECampaignBaseType.RELAY)
					ShowHint(EHint.CONFLICT_TOWER_SEIZING);
				else
					ShowHint(EHint.CONFLICT_BASE_SEIZING);
			}
		}
		else
		{
			// Entering a friendly base in a vehicle
			ChimeraCharacter player = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());

			if (player && player.IsInVehicle())
			{
				if (m_BaseWithPlayer.GetType() == SCR_ECampaignBaseType.BASE && !m_BaseWithPlayer.IsHQ())
					ShowHint(EHint.CONFLICT_SUPPLY_RUNS);
			}

			// Entering a friendly base with an armory
			if (GetGame().GetWorld().GetWorldTime() > SCR_GameModeCampaign.BACKEND_DELAY && m_BaseWithPlayer.GetServiceDelegateByType(SCR_EServicePointType.ARMORY))
			{
				ShowHint(EHint.CONFLICT_LOADOUTS);
				ShowHint(EHint.GAMEPLAY_RADIO_RESPAWN);
			}

			// Entering a friendly base covered by enemy radio signal
			bool covered;

			if (SCR_FactionManager.SGetLocalPlayerFaction() == m_Campaign.GetFactionByEnum(SCR_ECampaignFaction.BLUFOR))
				covered = base.IsHQRadioTrafficPossible(m_Campaign.GetFactionByEnum(SCR_ECampaignFaction.OPFOR));
			else
				covered = base.IsHQRadioTrafficPossible(m_Campaign.GetFactionByEnum(SCR_ECampaignFaction.BLUFOR));

			if (covered)
				ShowHint(EHint.CONFLICT_DEFENDING_BASES);
		}*/
	}
	void RecordBaseEnteredEvent(SCR_CampaignMilitaryBaseComponent base, IEntity Char)
	{
		if (!base.m_bVisited)
		{
			SCR_ChimeraCharacter ChimChar = SCR_ChimeraCharacter.Cast(Char);
			SP_CallendarComponent Callendar = ChimChar.GetCallendar();
			if (Callendar)
			{
				Callendar.RecordEvent("I arrived at " + base.GetAreaDesc());
				base.m_bVisited = true;
			}
		}
		else
		{
			SCR_ChimeraCharacter ChimChar = SCR_ChimeraCharacter.Cast(Char);
			SP_CallendarComponent Callendar = ChimChar.GetCallendar();
			if (Callendar)
			{
				Callendar.RecordEvent("I came back at " + base.GetBaseName());
			}
		}
	}
	void SendGuardToPlayer(SCR_AIGroup group)
	{
		array <AIAgent> chars = {};
		AIAgent closestguard;
		group.GetAgents(chars);
		if (chars.IsEmpty())
		{
			GetGame().GetCallqueue().CallLater(SendGuardToPlayer, 1000, false, group);
			return;
		}
		float dist;
		PlayerController playerController = GetGame().GetPlayerController();
		foreach (AIAgent agent : chars)
		{
			
			if (!dist)
			{
				dist = vector.Distance(playerController.GetControlledEntity().GetOrigin(), agent.GetControlledEntity().GetOrigin());
				closestguard = agent;
			}
			float newdist = vector.Distance(playerController.GetControlledEntity().GetOrigin(), agent.GetControlledEntity().GetOrigin());
			if (newdist < dist)
			{
				dist = newdist;
				closestguard = agent;
			}
		}
		if (!closestguard)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(closestguard.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		SCR_AITalkToCharacterBehavior action = new SCR_AITalkToCharacterBehavior(utility, null, playerController.GetControlledEntity(), false);
		utility.AddAction(action);
	}
}
