//------------------------------------------------------------------------------------------------

class KLM_ConsumablePredicate : InventorySearchPredicate
{
	SCR_EConsumableType m_ConsumableFilter;
	
	void KLM_ConsumablePredicate(SCR_EConsumableType type)
	{
		QueryComponentTypes.Insert(SCR_ConsumableItemComponent);
		m_ConsumableFilter = type;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return (SCR_ConsumableItemComponent.Cast(queriedComponents[0]).GetConsumableType() == m_ConsumableFilter);
	}
}

class PrefabResource_Predicate : InventorySearchPredicate
{
	ResourceName m_WantedPrefabName;
	void PrefabResource_Predicate(ResourceName prefabName)
	{
		m_WantedPrefabName = prefabName;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return item.GetPrefabData().GetPrefabName() == m_WantedPrefabName;
	}
}

//#define DEBUG_TRADER_ACTIONS 1

class KLM_BaseTraderAction : ScriptedUserAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_WantedPay;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_WantedAmount;
	[Attribute("-1", UIWidgets.EditBox, params: "-1 1000", desc: "")]
	int m_MaxUses;
	
	IEntity m_pOwner = null;
	int m_UsesCount = 0;	
	
	bool HasUses()
	{
		return (m_MaxUses == -1) || (m_UsesCount < m_MaxUses);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_pOwner = pOwnerEntity;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (RplSession.Mode() == RplMode.Client)
		{
			m_UsesCount++;
			return;	
		}			
		
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return;		
		
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_WantedPay);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		
		int movedCount = 0;
		ref array<IEntity> givenItems = {};
		
		for (int i = 0; i < entitiesToDrop.Count(); i++)
		{
			IEntity item = entitiesToDrop[i];
			if (!item) continue;
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			bool removed = inv.TryRemoveItemFromStorage(item,parentSlot.GetStorage());
			if (removed)
			{
				movedCount++;				
				givenItems.Insert(item);
				
				if (movedCount >= m_WantedAmount)
					break;
			}
		}				
		
		// todo: what to do next
		if (movedCount >= m_WantedAmount)
		{
			m_UsesCount++;
			Sell(pOwnerEntity, pUserEntity);
		
#ifdef DEBUG_TRADER_ACTIONS
			Print(string.Format("Trader Action processed. Obj moved %1. All possible obj %2. Deleted %3.", movedCount, entitiesToDrop.Count(), givenItems.Count()));
#endif		
			for (int i = 0; i < movedCount; i++)
			{			
				delete givenItems.Get(i);
			}
		}
		givenItems.Clear();		
	}
	
	bool Sell(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		CharacterControllerComponent contr = CharacterControllerComponent.Cast(m_pOwner.FindComponent(CharacterControllerComponent));
		if (contr)
			return !contr.IsDead();
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
#ifdef DEBUG_TRADER_ACTIONS	
		Print(string.Format("Can be performed script, uses %1. maxUses %2", m_UsesCount, m_MaxUses));		
#endif			
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
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	//! If HasLocalEffectOnly() is true this method tells if the server is supposed to broadcast this action to clients.
	override event bool CanBroadcastScript() 
	{ 
		return true; 
	};

}
