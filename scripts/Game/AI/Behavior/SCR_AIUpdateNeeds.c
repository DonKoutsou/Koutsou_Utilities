class SCR_AIUpdateNeeds : AITaskScripted
{
	override bool VisibleInPalette() { return true; }
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(owner.GetControlledEntity());
		if (Char)
			Char.UpdateNeeds();
		return ENodeResult.SUCCESS;
	}
}