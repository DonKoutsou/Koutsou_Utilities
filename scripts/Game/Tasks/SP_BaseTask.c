//------------------------------------------------------------------------------------------------
class SP_BaseTaskClass: SCR_ScenarioFrameworkTaskClass
{
};
class SP_BaseTask : SCR_ScenarioFrameworkTask
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
			GetTaskIconkWidget().SetColor(GetTargetFaction().GetFactionColor());
		else
			GetTaskIconkWidget().SetColor(Color.White);
	}
	void SetTarget(IEntity targtoset)
	{
		m_etargerent = targtoset;
	}
};
