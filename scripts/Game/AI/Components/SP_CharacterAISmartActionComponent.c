class SP_CharacterAISmartActionComponentClass: SCR_AISmartActionComponentClass
{
};


class SP_CharacterAISmartActionComponent : SCR_AISmartActionComponent
{
	void SP_CharacterAISmartActionComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Owner = GenericEntity.Cast(ent);
	}
	
};
class SP_StoreAISmartActionComponentClass: SCR_AISmartActionComponentClass
{
};


class SP_StoreAISmartActionComponent : SCR_AISmartActionComponent
{
	ADM_ShopComponent Shop;
	
	ref array <ERequestRewardItemDesctiptor> shoplist = {};
	
	bool TestDescriptor(ERequestRewardItemDesctiptor Descriptor)
	{
		if (shoplist.Contains(Descriptor))
			return true;
		return false;
	}
	
	void SP_StoreAISmartActionComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Owner = GenericEntity.Cast(ent);
		Shop = ADM_ShopComponent.Cast(ent.FindComponent(ADM_ShopComponent));
		if (!Shop)
			return;
		
		array <ref ADM_ShopMerchandise> Merchandise = Shop.GetMerchandise();
		if (Merchandise.IsEmpty())
			return;
		
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		
		foreach (ADM_ShopMerchandise merch : Merchandise)
		{
			ResourceName prefab = merch.GetMerchandise().GetPrefab();
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefab);
			if (entry)
			{
				ERequestRewardItemDesctiptor descr;
				SP_RequestData data = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
				if (data)
				{
					descr = data.GetRequestDescriptor();
				}
				SP_RequestAmmoData ammodata = SP_RequestAmmoData.Cast(entry.GetEntityDataOfType(SP_RequestAmmoData));
				if (ammodata)
				{
					descr = ammodata.GetRequestDescriptor();
				}
				if (!shoplist.Contains(descr))
				{
					shoplist.Insert(descr);
				}
			}
		}
	}
	
};

