/*modded class SCR_AISetFollowFalse : SCR_AIActionTask
{

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AIActionBase action = GetExecutedAction();
		
		if (!action)
			return ENodeResult.FAIL;
		SCR_AIFollowBehavior follow = SCR_AIFollowBehavior.Cast(action);
		if (follow){follow.SetActiveFollowing(false)};
		SCR_AITaskPickupBehavior pickup = SCR_AITaskPickupBehavior.Cast(action);
		if (pickup){pickup.SetActiveFollowing(false)};
		SCR_AIExecuteNavigateTaskBehavior nav = SCR_AIExecuteNavigateTaskBehavior.Cast(action);
		if (nav){nav.SetActiveFollowing(false)};
		SCR_AIExecuteDeliveryTaskBehavior del = SCR_AIExecuteDeliveryTaskBehavior.Cast(action);
		if (del){del.SetActiveFollowing(false)};
		return ENodeResult.SUCCESS;
	}
};