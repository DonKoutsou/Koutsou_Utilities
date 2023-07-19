//------------------------------------------------------------------------------------------------

class SP_CraftActionNotake : SP_BaseTraderActionNotake
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ref array <ResourceName> m_AltWantedPay;
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_CraftedObject;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_CraftedNumber;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;	
	[Attribute("false", UIWidgets.CheckBox, params: "", desc: "Should remove parent object after action")]
	bool m_bShouldRemoveParent;
	
	[Attribute(desc: "Missing item message")]
	string m_MissingItem;
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return;		
		
		PrefabResource_Predicate pred = new PrefabResource_Predicate(m_WantedPay);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		if(entitiesToDrop.Count() == 0)
		{
			for (int i = 0; i < m_AltWantedPay.Count(); i++)
			{
				PrefabResource_Predicate Altpred = new PrefabResource_Predicate(m_AltWantedPay[i]);
				inv.FindItems(entitiesToDrop, Altpred);
			}
		}
		int movedCount = 0;
		
		for (int i = 0; i < entitiesToDrop.Count(); i++)
		{
			IEntity item = entitiesToDrop[i];
			if (!item) continue;
			movedCount++;
		}		
		m_UsesCount++;
		
		// todo: what to do next
		if (movedCount >= m_WantedAmount)
		{
			Sell(pOwnerEntity, pUserEntity);
		}
	}
	override bool CanBePerformedScript(IEntity user)
	{
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
		if(entitiesToDrop.Count() == 0)
		{
			for (int i = 0; i < m_AltWantedPay.Count(); i++)
			{
				PrefabResource_Predicate Altpred = new PrefabResource_Predicate(m_AltWantedPay[i]);
				inv.FindItems(entitiesToDrop, Altpred);
			}
		}
		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			m_sCannotPerformReason = "Missing" + " " + m_MissingItem;
			return false;
		}			
		return true;
	}
	override bool Sell(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		vector mat[4];
		pOwnerEntity.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		
		Resource res = Resource.Load(m_CraftedObject);
		if (res)
		{	
			for (int j = 0; j < m_CraftedNumber; j++)		
				GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);
			
			if (m_bShouldRemoveParent && !HasUses())
				delete pOwnerEntity;
			
			return true;
		}
		return false;
	}
}
