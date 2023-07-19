class SP_AILootBodyAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		InventoryStorageManagerComponent stocompKiller = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		foreach (IEntity item : items)
		{
			SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
			SCR_ConsumableItemComponent cons = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
			if (Unretr || cons)
				{
					inv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
				}
		}
		SP_CharacterAISmartActionComponent SmartComp = SP_CharacterAISmartActionComponent.Cast(pOwnerEntity.FindComponent(SP_CharacterAISmartActionComponent));
		if(SmartComp)
			{
				SmartComp.SetActionAccessible(false);
			}
	}
	
}