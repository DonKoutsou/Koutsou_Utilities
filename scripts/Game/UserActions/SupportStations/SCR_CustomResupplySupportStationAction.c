
//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class SCR_CustomResupplyCatalogItemSupportStationData : SCR_ResupplySupportStationData
{
	protected SCR_ESupportStationResupplyType m_eResupplyType;
	
	[Attribute("0", desc: "If true will use the item associated with the support station default faction. Otherwise will only take defualt if no faction is set")]
	protected bool m_bAlwaysTakeDefaultFaction;
	
	protected ref map<FactionKey, ResourceName> m_mFactionItems = new map<FactionKey, ResourceName>;
		
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity owner)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return;
		
		array<Faction> factions = {};
		factionManager.GetFactionsList(factions);
		
		SCR_Faction scrFaction;
		SCR_EntityCatalog itemCatalog;
		
		array<SCR_EntityCatalogEntry> filteredEntityList = {};
		array<SCR_BaseEntityCatalogData> dataList = {};
		SCR_EntityCatalogSupportStationResupplyData resupplyData;
		int count;
		
		//~ Go over each faction and set which items will be used for the resupply
		foreach (Faction faction : factions)
		{
			scrFaction = SCR_Faction.Cast(faction);
			if (!scrFaction)
				continue;
			
			itemCatalog = scrFaction.GetFactionEntityCatalogOfType(EEntityCatalogType.ITEM);
			if (!itemCatalog)
				continue;
			
			count = itemCatalog.GetEntityListWithData(SCR_EntityCatalogSupportStationResupplyData, filteredEntityList, dataList);
			if (count <= 0)
				continue;
			
			for (int i = 0; i < count; i++)
			{
				resupplyData = SCR_EntityCatalogSupportStationResupplyData.Cast(dataList[i]);
				if (!resupplyData || resupplyData.GetResupplyType() != m_eResupplyType)
					continue;
				
				m_mFactionItems.Insert(faction.GetFactionKey(), filteredEntityList[i].GetPrefab());
				break;
			}
		}
	}
	void SetResupplyType(SCR_ESupportStationResupplyType Type)
	{
		m_eResupplyType = Type;
	}
	//------------------------------------------------------------------------------------------------
	override bool GetResupplyItemOrMuzzle(notnull IEntity targetCharacter, notnull IEntity actionOwner, SCR_SupportStationGadgetComponent supportStationGadget, out ResourceName item, out BaseMuzzleComponent muzzle)
	{
		if (m_mFactionItems.IsEmpty())
			return false;
		
		FactionAffiliationComponent factionAffiliationComponent = GetFactionAffiliationGadgetOrOwner(actionOwner, supportStationGadget);
		if (!factionAffiliationComponent)
			return false;
		
		//~ Get faction
		Faction faction;
		if (m_bAlwaysTakeDefaultFaction)
		{
			faction = factionAffiliationComponent.GetDefaultAffiliatedFaction();
			if (!faction)
				return false;
		}
		else 
		{
			faction = factionAffiliationComponent.GetAffiliatedFaction();
			if (!faction)
			{
				faction = factionAffiliationComponent.GetDefaultAffiliatedFaction();
				if (!faction)
					return false;
			}
		}	
	
		if (!m_mFactionItems.Find(faction.GetFactionKey(), item))
			return false;
		
		return !SCR_StringHelper.IsEmptyOrWhiteSpace(item);
	}
}