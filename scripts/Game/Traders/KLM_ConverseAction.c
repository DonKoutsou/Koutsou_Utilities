//------------------------------------------------------------------------------------------------

class KLM_ConverseAction : KLM_BaseTraderAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_PrefabToSell;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_SellingAmount;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;	
	
	override bool Sell(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		vector mat[4];
		pOwnerEntity.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		
		Resource res = Resource.Load(m_PrefabToSell);
		if (res)
		{	
			for (int j = 0; j < m_SellingAmount; j++)		
				GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);
			return true;
		}
		return false;
	}
	override bool CanBePerformedScript(IEntity user)
	{
#ifdef DEBUG_TRADER_ACTIONS	
		Print(string.Format("Can be performed script, uses %1. maxUses %2", m_UsesCount, m_MaxUses));		
#endif
		FactionAffiliationComponent UserFComp = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent SellerFComp = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
		FactionKey userkey = UserFComp.GetAffiliatedFaction().GetFactionKey();
		FactionKey sellerkey = SellerFComp.GetDefaultAffiliatedFaction().GetFactionKey();
		if(userkey && sellerkey && userkey != sellerkey)
		{
			m_sCannotPerformReason = "not part of faction";
			return false;
		}
		if (!HasUses())
		{
			m_sCannotPerformReason = "currently unavailable";
			return false;
		}
		
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;
		
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_WantedPay);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);

		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			m_sCannotPerformReason = "not enough";
			return false;
		}			
		return true;
	}
}
