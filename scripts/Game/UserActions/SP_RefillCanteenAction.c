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
				int initcap = ConsComp.GetInitialCapacity();
				float currentcap = ConsComp.GetCurrentCapacity();
				if (currentcap < initcap)
				{
					ConsComp.Refill();
					break;
				}
			}
		}
	}

	event override bool CanBePerformedScript(IEntity user) 
	{ 
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inv)
			return false;
		array<IEntity> items = new array<IEntity>();
		
		inv.GetItems(items);
		
		bool hascanteens;
		
		foreach (IEntity item : items)
		{
			SCR_ConsumableItemComponent ConsComp = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
			if (ConsComp && ConsComp.IsDrinkable())
			{
				hascanteens = true;
				int initcap = ConsComp.GetInitialCapacity();
				float currentcap = ConsComp.GetCurrentCapacity();
				if (currentcap < initcap)
				{
					return true;
				}
			}
		}
		if (hascanteens)
		{
			SetCannotPerformReason("All bottles are full");
			return false;
		}
		SetCannotPerformReason("No items to refill in inventory");
		return false;
	};
};