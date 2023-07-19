[EntityEditorProps(category: "GameScripted/GameMode", description: "Spawn point entity", visible: false)]
class SP_SingleUseSpawnPointClass : SCR_SpawnPointClass
{
	// [Attribute()]
	// protected ref SCR_UIInfo m_Info;

	// SCR_UIInfo GetInfo()
	// {
	// 	return m_Info;
	// }
};

//------------------------------------------------------------------------------------------------
//! Spawn point entity defines positions on which players can possibly spawn.
class SP_SingleUseSpawnPoint : SCR_SpawnPoint
{
	//override event void EOnPlayerSpawn(IEntity entity)
	//{
	//	delete this;
	//}
};
