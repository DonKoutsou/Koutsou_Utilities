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
	SCR_ArsenalComponent Shop;
	
	ref array <ERequestRewardItemDesctiptor> shoplist = {};
	ref array <ref BaseMagazineWell > MagList = {};
	bool TestNeeds(SCR_ChimeraCharacter Char)
	{
		if (shoplist.IsEmpty())
			return false;
		int ammount;
		BaseMagazineComponent mag;
		foreach (ERequestRewardItemDesctiptor need : shoplist)
		{
			if (Char.Checkneed(need, ammount, mag))
			{
				return true;
			}
		}
		return false;
	}
	void Init(IEntity owner)
	{
		m_Owner = GenericEntity.Cast(owner);
		Shop = SCR_ArsenalComponent.Cast(owner.FindComponent(SCR_ArsenalComponent));
		if (!Shop)
			return;
		
		array <SCR_ArsenalItem> Merchandise = {};
		Shop.GetFilteredArsenalItems(Merchandise);
		if (Merchandise.IsEmpty())
			return;
		
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		
		foreach (SCR_ArsenalItem merch : Merchandise)
		{
			ResourceName prefab = merch.GetItemResourceName();
			SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefab);
			if (entry)
			{				ERequestRewardItemDesctiptor descr;
				SP_RequestData data = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
				if (data)
				{
					descr = data.GetRequestDescriptor();
				}
				SP_RequestAmmoData ammodata = SP_RequestAmmoData.Cast(entry.GetEntityDataOfType(SP_RequestAmmoData));
				if (ammodata)
				{
					descr = ammodata.GetRequestDescriptor();
					if (!MagList.Contains(ammodata.GetMagType()))
						MagList.Insert(ammodata.GetMagType());
				}
				if (!shoplist.Contains(descr))
				{
					shoplist.Insert(descr);
				}
			}
		}
	}
	void SP_StoreAISmartActionComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		GetGame().GetCallqueue().CallLater(Init, 100, false, ent);
	}
	
};

