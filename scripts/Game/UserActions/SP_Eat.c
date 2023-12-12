class SP_EatAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		if (statComponent)
			statComponent.Eat(pOwnerEntity);
	}
		
}