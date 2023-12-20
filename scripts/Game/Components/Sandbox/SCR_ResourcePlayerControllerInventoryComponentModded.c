modded class SCR_ResourcePlayerControllerInventoryComponent
{
	SCR_CampaignMilitaryBaseManager MilMan;
	//------------------------------------------------------------------------------------------------
	override void OnPlayerInteraction(EResourcePlayerInteractionType interactionType, SCR_ResourceComponent resourceComponentFrom, SCR_ResourceComponent resourceComponentTo, EResourceType resourceType, float resourceValue)
	{
		super.OnPlayerInteraction(interactionType, resourceComponentFrom, resourceComponentTo, resourceType, resourceValue);
		CheckForSteal(resourceComponentFrom, resourceValue);
		
		
	}
	void CheckForSteal(SCR_ResourceComponent resourceComponentFrom, float ammount = 1)
	{
		if (!resourceComponentFrom.m_bCheckForSteal)
		{
			return;
		}
		if (!MilMan)
		{
			MilMan = SCR_CampaignMilitaryBaseManager.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager());
			
		}
		if (!MilMan)
			return;
		
		IEntity owner = GetOwner();
		PlayerController playerController = PlayerController.Cast(owner);
		
		SCR_CampaignMilitaryBaseComponent base = MilMan.GetClosestBase(playerController.GetControlledEntity().GetOrigin());
		if (!base)
			return;
		
		if (!base.GetIsEntityInMyRange(playerController.GetControlledEntity()))
			return;
		
		if (!base.IsInhabited())
			return;
		
		FactionAffiliationComponent AffComp = FactionAffiliationComponent.Cast(playerController.GetControlledEntity().FindComponent(FactionAffiliationComponent));
		
		SCR_CampaignFaction basefaction = SCR_CampaignFaction.Cast(base.GetFaction());
		SCR_CampaignFaction plfaction = SCR_CampaignFaction.Cast(AffComp.GetAffiliatedFaction());
		
		if (basefaction == plfaction || !basefaction.IsFactionFriendly(plfaction))
			return;
		
		basefaction.AdjustPlayerGoodwill(-ammount, SP_EGoodWillReason.STEAL);
		SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Stealing supplies from %1 decreased your goodwill towards them.", basefaction.GetFactionKey()));
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override void RpcAsk_ArsenalRequestItem(RplId rplIdResourceComponent, RplId rplIdInventoryManager, RplId rplIdStorageComponent, ResourceName resourceNameItem, EResourceType resourceType)
	{
		super.RpcAsk_ArsenalRequestItem(rplIdResourceComponent, rplIdInventoryManager, rplIdStorageComponent, resourceNameItem, resourceType);
		
		if (!rplIdResourceComponent.IsValid())
			return;
		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(Replication.FindItem(rplIdResourceComponent));
		if (!resourceComponent)
			return;
		float resourceCost = 0;
		
		//~ Get item cost
		SCR_EntityCatalogManagerComponent entityCatalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (entityCatalogManager)
		{
			IEntity resourcesOwner = resourceComponent.GetOwner();
			if (!resourcesOwner)
				return;
			
			SCR_ArsenalComponent arsenalComponent = SCR_ArsenalComponent.Cast(resourcesOwner.FindComponent(SCR_ArsenalComponent));
			SCR_Faction faction;
			if (arsenalComponent)
				faction = arsenalComponent.GetAssignedFaction();
			
			SCR_EntityCatalogEntry entry;
			
			if (faction)
				 entry = entityCatalogManager.GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM, resourceNameItem, faction);
			else 
				entry = entityCatalogManager.GetEntryWithPrefabFromCatalog(EEntityCatalogType.ITEM, resourceNameItem);
			
			if (entry)
			{
				SCR_ArsenalItem data = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
				if (data)
					resourceCost = data.GetSupplyCost();
			}
		}
		CheckForSteal(resourceComponent, resourceCost);
	}

}