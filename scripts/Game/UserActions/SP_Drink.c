class SP_DrinkAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		if (statComponent)
			statComponent.Drink(playerController);
	}
		
}