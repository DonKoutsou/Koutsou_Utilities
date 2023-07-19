class SP_OpenFactionStorageAction : SCR_InventoryAction
{
	FactionKey m_StorageFactKey;
	#ifndef DISABLE_INVENTORY
	//------------------------------------------------------------------------------------------------
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		auto vicinity = CharacterVicinityComponent.Cast( pUserEntity .FindComponent( CharacterVicinityComponent ));
		if ( !vicinity )
			return;
		manager.SetStorageToOpen(pOwnerEntity);
		manager.OpenInventory();
	}
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_Item = InventoryItemComponent.Cast( pOwnerEntity .FindComponent( InventoryItemComponent ));
		FactionAffiliationComponent StorageFactComp = FactionAffiliationComponent.Cast(pOwnerEntity.FindComponent(FactionAffiliationComponent));
		if (StorageFactComp)
		{
			m_StorageFactKey = StorageFactComp.GetAffiliatedFaction().GetFactionKey();
		}
	}
	#endif
	override bool CanBePerformedScript(IEntity user)
 	{
		if (!user)
			return false;
		Managed genericInventoryManager = user.FindComponent( SCR_InventoryStorageManagerComponent );
		if (!genericInventoryManager)
			return false;
		RplComponent genericRpl = RplComponent.Cast(user.FindComponent( RplComponent ));
		if (!genericRpl)
			return false;
		FactionAffiliationComponent FactComp = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		if (m_StorageFactKey && m_StorageFactKey != FactComp.GetAffiliatedFaction().GetFactionKey())
		{
			m_sCannotPerformReason = "Not part of" + " " + m_StorageFactKey + "faction";
			return false;
		}
		return genericRpl.IsOwner();
	}
	
};