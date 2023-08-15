modded class SCR_AIToggleFlashlights
{
	//------------------------------------------------------------------------------------------------------
	override static ENodeResult ToggleFlashlights(notnull SCR_GadgetManagerComponent gadgetManager, bool newState)
	{
		array<SCR_GadgetComponent> gadgets = gadgetManager.GetGadgetsByType(EGadgetType.FLASHLIGHT);
		
		if (!gadgets)
			return ENodeResult.FAIL;
		
		if (gadgets.IsEmpty())
			return ENodeResult.FAIL;
		
		bool success = false;
		foreach (SCR_GadgetComponent gadget : gadgets)
		{
			if (gadget.IsActive() == newState)
				continue;
			InventoryItemComponent invComp = InventoryItemComponent.Cast(gadget.GetOwner().FindComponent(InventoryItemComponent));
			
			if (!invComp)
				continue;
			
			EquipmentStorageSlot slot = EquipmentStorageSlot.Cast(invComp.GetParentSlot());
			
			// Don't toggle gadgets not in slot
			if (!slot)
				continue;
			
			bool occluded = slot.IsOccluded();
			
			gadget.ToggleActive(newState && !occluded);
			success |= true;
		}
			
		if (success)
			return ENodeResult.SUCCESS;
		else
			return ENodeResult.FAIL;
	}
}