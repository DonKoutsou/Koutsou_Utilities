
//------------------------------------------------------------------------------------------------
//! Spawn point entity defines positions on which players can possibly spawn.
modded class SCR_SpawnPoint
{
	//------------------------------------------------------------------------------------------------
	//! Get spawn points valid for factions friendly to and of given faction
	//! \param factionKey Valid faction key
	static array<SCR_SpawnPoint> GetSpawnPointsForFactionNFriends(string factionKey)
	{
		SCR_FactionManager factionman = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		array<SCR_SpawnPoint> factionSpawnPoints = new array<SCR_SpawnPoint>();
		if (factionKey.IsEmpty())
			return factionSpawnPoints;

		array<SCR_SpawnPoint> spawnPoints = GetSpawnPoints();
		foreach (SCR_SpawnPoint spawnPoint : spawnPoints)
		{
			
			if (spawnPoint)
			{
				
				if (spawnPoint.GetFactionKey() == factionKey)
				{
					factionSpawnPoints.Insert(spawnPoint);
					return factionSpawnPoints;
				}
				SCR_Faction faction = SCR_Faction.Cast(factionman.GetFactionByKey(spawnPoint.GetFactionKey()));
				SCR_Faction Myfaction = SCR_Faction.Cast(factionman.GetFactionByKey(factionKey));
				if (faction && faction.IsFactionFriendly(Myfaction))
				{
					factionSpawnPoints.Insert(spawnPoint);
				}
			}
				
		}
		return factionSpawnPoints;
	}
	static SCR_SpawnPoint GetRandomSpawnPointForFactionNFriends(string factionKey)
	{
		array<SCR_SpawnPoint> spawnPoints = GetSpawnPointsForFactionNFriends(factionKey);
		if (!spawnPoints.IsEmpty())
			return spawnPoints.GetRandomElement();

		return null;
	}
}
