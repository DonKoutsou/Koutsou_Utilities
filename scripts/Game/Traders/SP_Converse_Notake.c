//------------------------------------------------------------------------------------------------

class LIW_ConsumablePredicateNoTake : InventorySearchPredicate
{
	SCR_EConsumableType m_ConsumableFilter;
	
	void LIW_ConsumablePredicate(SCR_EConsumableType type)
	{
		QueryComponentTypes.Insert(SCR_ConsumableItemComponent);
		m_ConsumableFilter = type;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return (SCR_ConsumableItemComponent.Cast(queriedComponents[0]).GetConsumableType() == m_ConsumableFilter);
	}
}

class PrefabResource_PredicateNoTake : InventorySearchPredicate
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

class SP_ConverseActionNoTake : ScriptedUserAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_WantedPay;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_WantedAmount;
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_PrefabToSell;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_SellingAmount;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	[Attribute("", UIWidgets.EditBox, params: "", desc: "")]
	string m_UnavailableText;
		
	ref array<IEntity> m_GivenItems = {};
	IEntity m_pOwner = null;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_pOwner = pOwnerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// clear list of passed items
		m_GivenItems.Clear();
		
		// find inventory of user (player)
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return;		
		
		// find wanted items in user inventory
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_WantedPay);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		
		// iterate through found items
		int movedCount = 0;
		for (int i = 0; i < entitiesToDrop.Count(); i++)
		{
			IEntity item = entitiesToDrop[i];
			if (!item) continue;
		}		

		// todo: what to do next
		if (inv >= m_WantedAmount)
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
			}
		}
		
		// delete given items
		for (int i = 0; i < movedCount; i++)
		{			
			delete m_GivenItems.Get(i);
		}
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
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;
		
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_WantedPay);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);

		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			m_sCannotPerformReason = m_UnavailableText;
			return false;
		}			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}

}