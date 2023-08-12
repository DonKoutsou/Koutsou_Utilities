
modded class SCR_FactionAffiliationComponent
{
	ref map <LoadoutSlotInfo, FactionKey> clothing = new map <LoadoutSlotInfo, FactionKey>();
	override event protected void OnPostInit(IEntity owner)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(owner);
		if (!char)
			return;
		SCR_CharacterInventoryStorageComponent inv = SCR_CharacterInventoryStorageComponent.Cast(owner.FindComponent(SCR_CharacterInventoryStorageComponent));
		ManageClothingChange(inv.GetClothFromArea(LoadoutHeadCoverArea), inv.GetSlotFromArea(LoadoutHeadCoverArea).GetID());
		ManageClothingChange(inv.GetClothFromArea(LoadoutPantsArea), inv.GetSlotFromArea(LoadoutPantsArea).GetID());
		ManageClothingChange(inv.GetClothFromArea(LoadoutJacketArea), inv.GetSlotFromArea(LoadoutJacketArea).GetID());
		inv.GetOnClothingUpdate().Insert(ManageClothingChange);
	};
	void ManageClothingChange(IEntity Item, int slotID)
	{
		SCR_CharacterInventoryStorageComponent inv = SCR_CharacterInventoryStorageComponent.Cast(GetOwner().FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!clothing.Contains(LoadoutSlotInfo.Cast(inv.GetSlot(slotID))))
		{
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(Item.FindComponent(InventoryItemComponent));
			if (!itemComponent) 
				return;
			
			SCR_ItemAttributeCollection attributes = SCR_ItemAttributeCollection.Cast(itemComponent.GetAttributes());
			if (!attributes)
				return;
			
			SP_ClothFactionAttributes factAttr = SP_ClothFactionAttributes.Cast(attributes.FindAttribute(SP_ClothFactionAttributes));
			if (!factAttr)
				return;
			clothing.Insert(LoadoutSlotInfo.Cast(inv.GetSlot(slotID)), factAttr.GetFaction());
			OnClothingAdded();
			return;
		}
	}
	void OnClothingAdded()
	{
		int fiacount;
		int uscount;
		int ussrcount;
		foreach (FactionKey key : clothing)
		{
			if (key == "US")
				uscount += 1;
			if (key == "USSR")
				ussrcount += 1;
			if (key == "FIA")
				fiacount += 1;
		}
		if (fiacount > 0)
		{
	   		if (uscount == 0 && ussrcount == 0)
			{
				SetAffiliatedFactionByKey("FIA");
	    	}
	    	else if (uscount > 0 || ussrcount > 0)
			{
				SetAffiliatedFactionByKey("RENEGADE");
	    	}
		}
		if (uscount > 0)
		{
	   		if (fiacount == 0 && ussrcount == 0)
			{
				SetAffiliatedFactionByKey("US");
	    	}
	    	else if (fiacount > 0 || ussrcount > 0)
			{
				SetAffiliatedFactionByKey("RENEGADE");
	    	}
		}
		if (ussrcount > 0)
		{
	   		if (uscount == 0 && fiacount == 0)
			{
				SetAffiliatedFactionByKey("USSR");
	    	}
	    	else if (uscount > 0 || fiacount > 0)
			{
				SetAffiliatedFactionByKey("RENEGADE");
	    	}
		}
		
	}
}
modded class SCR_CharacterInventoryStorageComponent
{
	private ref ScriptInvoker m_OnClothingUpdate = new ref ScriptInvoker();
	
	ScriptInvoker GetOnClothingUpdate()
	{
		return m_OnClothingUpdate;
	}
	
	override protected override void OnAddedToSlot(IEntity item, int slotID)
	{
		super.OnAddedToSlot(item, slotID);
		m_OnClothingUpdate.Invoke(item, slotID);
	}
	
}
class SP_ClothFactionAttributes: BaseItemAttributeData
{
	[Attribute()]
	private FactionKey m_Faction;
	
	//------------------------------------------------------------------------------------------------	
	FactionKey GetFaction() 
	{
		return m_Faction;
	}
};
