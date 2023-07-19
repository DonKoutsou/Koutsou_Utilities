class SP_DrinkAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(pUserEntity.FindComponent(SP_CharacterStatsComponent));
		if (statComponent)
			statComponent.Drink(pOwnerEntity);
	}
		
}