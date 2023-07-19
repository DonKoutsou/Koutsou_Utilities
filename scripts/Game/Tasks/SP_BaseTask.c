//------------------------------------------------------------------------------------------------
class SP_BaseTaskClass: SCR_BaseTaskClass
{
};
class SP_BaseTask : SCR_BaseTask
{
	[Attribute()]
	FactionKey m_faction;
	
	IEntity m_etargerent;
	//------------------------------------------------------------------------------------------------
	
	override void OnMapOpen(MapConfiguration config)
	{
		vector spawnpoint[4]; 
		m_etargerent.GetWorldTransform(spawnpoint);
		SetTransform(spawnpoint);
	}
	override void UpdateMapTaskIcon()
	{
		if (!GetTaskIconkWidget())
			return;
		FactionManager factionManager = GetGame().GetFactionManager();
		if (IsAssignedToLocalPlayer() || SCR_EditorManagerEntity.IsOpenedInstance(false))
			GetTaskIconkWidget().SetColor(factionManager.GetFactionByKey(m_faction).GetFactionColor());
		else
			GetTaskIconkWidget().SetColor(Color.White);
	}
	void SetTarget(IEntity targtoset)
	{
		m_etargerent = targtoset;
	}
};
