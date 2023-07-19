/*class ADM_ShopComponentClass: ScriptComponentClass {}
class ADM_ShopComponent: ADM_ShopBaseComponent
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
	ref array<ResourceName> m_Categories;
	
	ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
	
	[Attribute(defvalue: "", desc: "Merchandise to sell", uiwidget: UIWidgets.Object, category: "Shop")]
	protected ref array<ref ADM_ShopMerchandise> m_AdditionalMerchandise;
	
	array<ref ADM_ShopCategory> GetCategories()
	{
		return m_LoadedCategories;
	}
	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
		
		if (!m_Merchandise) 
			m_Merchandise = new array<ref ADM_ShopMerchandise>();
		
		if (m_ShopConfig != string.Empty) {
			ADM_ShopConfig shopConfig = ADM_ShopConfig.GetConfig(m_ShopConfig);
			if (shopConfig && shopConfig.m_Merchandise) {
				foreach (ADM_ShopMerchandise merch : shopConfig.m_Merchandise) {
					m_Merchandise.Insert(merch);
				}
			}
		}
		foreach (ADM_ShopMerchandise merch : m_AdditionalMerchandise) {
			m_Merchandise.Insert(merch);
		}
		
		foreach (ResourceName category : m_Categories)
		{
			m_LoadedCategories.Insert(ADM_ShopCategory.GetConfig(category));
		}
	}
	void ~ADM_ShopComponent() 
		{
			m_LoadedCategories.Clear();
			m_Merchandise.Clear();
    }
}