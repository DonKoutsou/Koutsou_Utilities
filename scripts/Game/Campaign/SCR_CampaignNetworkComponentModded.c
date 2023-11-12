modded enum ECampaignClientNotificationID
{
	BASE_ENTERED
};
modded class SCR_CampaignNetworkComponent
{
	override protected void PlayerFeedbackImpl(int msgID, float value = 0, int assetID = -1, int baseID = -1)
	{
		LocalizedString msg;
		LocalizedString msg2;
		int duration = 2;
		int prio = -1;
		string msg1param1;
		string msg2param1;
		string msg2param2;
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		SCR_CampaignMilitaryBaseComponent base = campaign.GetBaseManager().FindBaseByCallsign(baseID);
		SCR_CampaignFeedbackComponent manager = SCR_CampaignFeedbackComponent.GetInstance();
		
		if (!campaign)
			return;
		
		switch (msgID)
		{	
			case ECampaignClientNotificationID.SUPPLIES_LOADED:
			{
				msg = "#AR-Campaign_SuppliesLoaded-UC";
				duration = 6;
				prio = SCR_ECampaignPopupPriority.SUPPLIES_HANDLED;
				msg1param1 = value.ToString();
				
				if (base)
				{
					msg2 = "#AR-Campaign_SuppliesAmountInfo-UC";
					msg2param1 = base.GetBaseName();
					msg2param2 = base.GetSupplies().ToString();
				}
				
				SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_LOADSUPPLIES);
				break;
			};
			case ECampaignClientNotificationID.SUPPLIES_UNLOADED:
			{
				msg = "#AR-Campaign_SuppliesUnloaded-UC";
				duration = 6;
				prio = SCR_ECampaignPopupPriority.SUPPLIES_HANDLED;
				msg1param1 = value.ToString();
				
				if (base)
				{
					msg2 = "#AR-Campaign_SuppliesAmountInfo-UC";
					msg2param1 = base.GetBaseName();
					msg2param2 = base.GetSupplies().ToString();
				}
				
				if (!campaign.IsTutorial())
				{
					if (manager)
						manager.ShowHint(EHint.CONFLICT_BUILDING);
				}
				
				SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_UNLOADSUPPLIES);
				break;
			};
			case ECampaignClientNotificationID.RESPAWN:
			{
				//manager.SetIsPlayerInRadioRange(true);
				if (manager)
					manager.OnRespawn();
				
				if (!base)
					return;

				msg = base.GetBaseNameUpperCase();
				ChimeraWorld world = m_PlayerController.GetWorld();
				TimeAndWeatherManagerEntity timeManager = world.GetTimeAndWeatherManager();
				
				if (timeManager)
				{
					int hours;
					int minutes;
					int seconds;
					timeManager.GetHoursMinutesSeconds(hours, minutes, seconds);
					string strHours = hours.ToString();
					
					if (hours > 0 && hours < 10)
						strHours = "0" + strHours;
					
					string strMinutes = minutes.ToString();
					
					if (minutes < 10)
						strMinutes = "0" + strMinutes;
					
					msg = string.Format("%1, %2:%3", msg, strHours, strMinutes);
				}
				
				msg2 = SCR_BaseTask.TASK_HINT_TEXT;
				msg2param1 = SCR_PopUpNotification.TASKS_KEY_IMAGE_FORMAT;
				duration = 5;
				prio = SCR_ECampaignPopupPriority.RESPAWN;
				
				if (m_bFirstSpawn)
				{
					m_bFirstSpawn = false;
				}
				
				break;
			}; 			
			default: {return;};
		}
		
		SCR_PopUpNotification.GetInstance().PopupMsg(msg, duration, msg2, param1: msg1param1, text2param1: msg2param1, text2param2: msg2param2);
	}
}