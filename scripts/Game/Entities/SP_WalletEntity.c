class WalletEntityClass: GameEntityClass{};
class WalletEntity: GameEntity
{
	SCR_InventoryStorageManagerComponent m_cInventoryManager;
	IEntity Owner;
	ResourceName Drachma = "{891BA05A96D3A0BE}prefabs/Currency/Drachma.et";
	SCR_InventoryStorageManagerComponent GetInventoryManager(){return m_cInventoryManager;}
	override event protected void EOnInit(IEntity owner)
	{
		m_cInventoryManager = SCR_InventoryStorageManagerComponent.Cast(owner.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!m_cInventoryManager)
			Print("Missing inventory in your wallet cunt.");
		SpawnCurrency(Math.RandomInt(5, 20));
		if (GetParent())
			Owner = GetParent().GetParent();
	}
	bool AddCurrency(array <IEntity> currency)
	{
		int Movedamount;
		for (int i, count = currency.Count(); i < count; i++)
		{
			m_cInventoryManager.InsertItem(currency[i]);
			Movedamount += 1;
		}
		if (Movedamount == currency.Count())
		{
			return true;
		}
		else 
		{
			Print("Some currency wasnt added.");
			return true;
		}
		return false;
	}
	bool RemoveCurrency(out array<IEntity> Removed, int ammount)
	{
		array<IEntity> currency = {};
		GetCurrency(currency);
		if (currency.Count() < ammount)
			ammount = currency.Count();
		for (int i, count = ammount; i < count; i++)
		{
			m_cInventoryManager.TryRemoveItemFromInventory(currency[i]);
			Removed.Insert(currency[i]);
		}
		if (Removed.Count() == ammount)
			return true;
		return false;
	}
	void GetCurrency(out array<IEntity> currency)
	{
		SP_CurrencyPredicate pred = new SP_CurrencyPredicate();
		m_cInventoryManager.FindItems(currency, pred);
	}
	int GetCurrencyAmmount()
	{
		int ammount;
		array <IEntity> Drachmas = {};
		SP_CurrencyPredicate pred = new SP_CurrencyPredicate();
		ammount = m_cInventoryManager.FindItems(Drachmas, pred);
		return ammount;
	}
	void SpawnCurrency(int ammount)
	{
		for (int j = 0; j < ammount; j++)
			m_cInventoryManager.TrySpawnPrefabToStorage(Drachma);
	}
	bool TakeCurrency(WalletEntity Origin, int ammount)
	{
		array<IEntity> Removed = {};
		Origin.RemoveCurrency(Removed, ammount);
		if (AddCurrency(Removed))
			return true;
		return false;
	}
};
class SP_CurrencyPredicate : InventorySearchPredicate
{
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		InventoryItemComponent Inv = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!Inv)
			return false;
		if (Inv.GetAttributes().GetCommonType() == ECommonItemType.CURRENCY)
			return true;
		return false;
	}
}
class SP_WalletPredicate : InventorySearchPredicate
{
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		WalletEntity wallet = WalletEntity.Cast(item);
		if(wallet)
		{
			return true;
		}
		return false;
	}
}
modded enum ECommonItemType
{
	CURRENCY,
};