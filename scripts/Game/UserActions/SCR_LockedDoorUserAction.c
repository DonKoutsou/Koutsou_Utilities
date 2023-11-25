//! User action that ought to be attached to an entity with door component.
//! When performed either opens or closes the door based on the previous state of the door.
class SCR_LockedDoorUserAction : SCR_DoorUserAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_Key;
	
	override bool CanBePerformedScript(IEntity user)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;	
		
		DS_PrefabResource_Predicate pred = new DS_PrefabResource_Predicate(m_Key);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		if(!inv.FindItems(entitiesToDrop, pred))
		{
			m_sCannotPerformReason = "Missing Key";
			return false;
			
		}
		
		DoorComponent doorComponent = GetDoorComponent();
		if (doorComponent)
			return true;
		
		return false;
	}
};