class SP_RefillCanteensAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		foreach (IEntity item : items)
		{
			SCR_ConsumableItemComponent ConsComp = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
			if (ConsComp && ConsComp.IsDrinkable())
				{
					ConsComp.Refill();
				}
		}
	}
	/*override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Target character
		ChimeraCharacter targetCharacter = ChimeraCharacter.Cast(GetOwner());
		if (!targetCharacter)
			return;
		
		// Medic character
		ChimeraCharacter userCharacter = ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
		
		SCR_CharacterControllerComponent userController = SCR_CharacterControllerComponent.Cast(userCharacter.GetCharacterController());
		if (!userController)
			return;
		
		IEntity item = userController.GetAttachedGadgetAtLeftHandSlot();
		if (!item)
			return;
		
		SCR_ConsumableItemComponent consumableComponent = GetConsumableComponent(userCharacter);
		if (!consumableComponent)
			return;
		
		SCR_ConsumableEffectHealthItems consumableEffect = SCR_ConsumableEffectHealthItems.Cast(consumableComponent.GetConsumableEffect());
		if (!consumableEffect)
			return;
			
		TAnimGraphCommand desiredCmd = consumableEffect.GetApplyToOtherAnimCmnd(pOwnerEntity);

		SCR_CharacterControllerComponent targetController = SCR_CharacterControllerComponent.Cast(targetCharacter.GetCharacterController());
		if (targetController && targetController.IsUnconscious())
			desiredCmd = consumableEffect.GetReviveAnimCmnd(pOwnerEntity);
	
		SCR_CharacterDamageManagerComponent targetDamageMan = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!targetDamageMan)
			return;

		consumableComponent.SetTargetCharacter(pOwnerEntity);
		consumableComponent.GetConsumableEffect().ActivateEffect(pOwnerEntity, pUserEntity, item, new SCR_ConsumableEffectAnimationParameters(desiredCmd, 1, 0.0, consumableEffect.GetApplyToOtherDuraction(), targetDamageMan.FindAssociatedBandagingBodyPart(m_eHitZoneGroup), 0.0, false));
	}	*/
	event override bool CanBePerformedScript(IEntity user) 
	{ 
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		foreach (IEntity item : items)
		{
			SCR_ConsumableItemComponent ConsComp = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
			if (ConsComp && ConsComp.IsDrinkable())
				{
					return true;
				}
		}
		SetCannotPerformReason("No items to refill in inventory");
		return false;
	};
};