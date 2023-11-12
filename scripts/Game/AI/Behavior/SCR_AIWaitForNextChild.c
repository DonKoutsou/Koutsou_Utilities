class SCR_AIWaitForNextChild : AITaskScripted
{
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Checks if next member of group is getting spawned and returns running until he doese";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AIGroup group = SCR_AIGroup.Cast(owner);
		if (!group)
			return ENodeResult.SUCCESS;
		//if (!group.inited)
		//{
		//	return ENodeResult.RUNNING;
		//}
		if (group.GetAgentsCount() == 0)
		{
			return ENodeResult.RUNNING;
		}
		return ENodeResult.SUCCESS;
		
	}
}