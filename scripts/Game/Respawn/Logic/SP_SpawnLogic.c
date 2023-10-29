[BaseContainerProps(category: "Respawn")]
modded class SCR_AutoSpawnLogic
{
	[Attribute("1")]
	int Lives;
	override void OnPlayerKilled_S(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled_S(playerId, playerEntity, killerEntity, killer);
		Lives -= 1;
		if (Lives == 0)
		{
			SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
			if (!manager)
				return;
			manager.SetMissionEndScreen(EGameOverTypes.DEATH);
			manager.Finish();
			return;
		}
		
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawned_S(int playerId, IEntity entity)
	{
		super.OnPlayerSpawned_S(playerID, entiry);
		SCR_CampaignFactionManager factman = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
		affcomp.SetAffiliatedFaction(factman.GetFactionByKey("CIV"));
		SCR_PlayerFactionAffiliationComponent playerFactionAffiliation = GetPlayerFactionComponent_S(playerId);
		GetGame().GetCallqueue().CallLater( playerFactionAffiliation.RequestFaction , 1000, false, factman.GetFactionByKey("CIV"));
	}
};
