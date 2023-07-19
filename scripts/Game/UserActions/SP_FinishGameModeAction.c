class SP_FinishGameModeAction : ScriptedUserAction
{
	[Attribute()]
	EGameOverTypes m_GameoverType;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
		if (!manager)
			return;
		if (m_GameoverType)
			manager.SetMissionEndScreen(m_GameoverType);
		
		manager.Finish();
		
	}
	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}
};