class SP_EatAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(pUserEntity.FindComponent(SP_CharacterStatsComponent));
		if (statComponent)
			statComponent.Eat(pOwnerEntity);
	}
		
}