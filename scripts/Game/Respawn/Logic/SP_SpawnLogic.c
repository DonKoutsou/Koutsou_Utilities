[BaseContainerProps(category: "Respawn")]
class SP_SpawnLogic : SCR_AutoSpawnLogic
{
	[Attribute("1")]
	int Lives;
	override protected void DoSpawn(int playerId, SCR_SpawnData data)
	{
		Lives -= 1;
		if (!GetPlayerRespawnComponent_S(playerId).RequestSpawn(data))
		{
			// Try again later
		}
	}
	override protected void Spawn(int playerId)
	{
		/*if (Lives == 0)
		{
			SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
			if (!manager)
				return;
			manager.SetMissionEndScreen(EGameOverTypes.DEATH);
			manager.Finish();
			return;
		}*/
			
		super.Spawn(playerId);
		
	}
};
