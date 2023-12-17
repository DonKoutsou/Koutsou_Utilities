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
	
	void AddAssigneecustom(SCR_BaseTaskExecutor assignee, float timestamp, bool showmsg = false)
	{
		if (!m_aAssignees || !assignee)
			return;
		
		IEntity assigneeEntity = assignee.GetControlledEntity();
		if (assigneeEntity)
		{
			SCR_CharacterControllerComponent characterControllerComponent = SCR_CharacterControllerComponent.Cast(assigneeEntity.FindComponent(SCR_CharacterControllerComponent));
			if (characterControllerComponent)
				characterControllerComponent.GetOnPlayerDeath().Insert(OnAssigneeKilled);
		}
		else
		{
			SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			if (!gameMode)
				return;
			gameMode.GetOnPlayerSpawned().Insert(InitOnSpawn);
		}
		
		// Is this task for an individual & we already have someone assigned?
		if (m_bIndividualTask && m_aAssignees.Count() > 0)
			return;
		
		int index = m_aAssignees.Find(assignee);
		
		if (index != -1)
			return;
		
		SetLastAssigneeAddedTimestamp(timestamp);
		
		m_aAssignees.Insert(assignee);
		if (assignee == SCR_BaseTaskExecutor.GetLocalExecutor() && showmsg)
			ShowPopUpNotification("#AR-Tasks_AssignPopup");

		OnAssigneeAdded(assignee);
		
		SCR_BaseTaskManager.s_OnTaskAssigned.Invoke(this);
		
		RegisterTaskUpdate(SCR_ETaskEventMask.TASK_ASSIGNEE_CHANGED);
	}
};
