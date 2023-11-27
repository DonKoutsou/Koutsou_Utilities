class SP_UnretrievableComponentClass : GameComponentClass
{
};

class SP_UnretrievableComponent : GameComponent
{
};
class SP_PackageComponentClass : SP_UnretrievableComponentClass
{
};

class SP_PackageComponent : SP_UnretrievableComponent
{
	string OwnerName;
	string DeliveryName;
	string DeliveryLocation;
	
	void GetInfo(out string OName, out string DName, out string DLoc)
	{
		OName = OwnerName;
		DName = DeliveryName;
		DLoc = DeliveryLocation;
	};
	void SetInfo(string OName, string DName, string DLoc)
	{
		OwnerName = OName;
		DeliveryName = DName;
		DeliveryLocation = DLoc;
	}
};
class SP_BountyComponentClass : SP_UnretrievableComponentClass
{
};

class SP_BountyComponent : SP_UnretrievableComponent
{
	string OwnerName;
	string TargetName;
	string TargetLocation;
	
	void GetInfo(out string OName, out string DName, out string TLoc)
	{
		OName = OwnerName;
		DName = TargetName;
		TLoc = TargetLocation;
	};
	void SetInfo(string OName, string DName, string TLoc)
	{
		OwnerName = OName;
		TargetName = DName;
		TargetLocation = TLoc;
	}
	string GetTargetName()
	{
		if(TargetName)
		{
			return TargetName;
		}
		return STRING_EMPTY;
	}
};
class SP_ItemBountyComponentClass : SP_UnretrievableComponentClass
{
};

class SP_ItemBountyComponent : SP_UnretrievableComponent
{
	string OwnerName;
	int ItemAmount;
	string OwnerLocation;
	SCR_EArsenalItemType	RequestType;
	SCR_EArsenalItemMode	RequestMode;
	EEditableEntityLabel	RequestLabel;

	void GetInfo(out string OName, out SCR_EArsenalItemType type, out SCR_EArsenalItemMode mode, out EEditableEntityLabel label, out int Amount, out string TLoc)
	{
		OName = OwnerName;
		type = RequestType;
		mode = RequestMode;
		label = RequestLabel;
		Amount = ItemAmount;
		TLoc = OwnerLocation;
	};
	void SetInfo(string OName, SCR_EArsenalItemType type, SCR_EArsenalItemMode mode , EEditableEntityLabel label, int Amount, string TLoc)
	{
		OwnerName = OName;
		RequestType = type;
		RequestMode = mode;
		RequestLabel = label;
		ItemAmount = Amount;
		OwnerLocation = TLoc;
	}
};
modded enum EEntityCatalogType
{
	REQUEST 						= 1000,
	REWARD							= 1200,
	STASH							= 1400,
	SPAWNPOINT						= 1500,
	DIRECTOR						= 1600,
}
[BaseContainerProps()]
class SCR_PackageInventoryItemHintUIInfo : SCR_InventoryItemHintUIInfo
{
	//------------------------------------------------------------------------------------------------
	override bool SetItemHintNameTo(InventoryItemComponent item, TextWidget textWidget)
	{
		if (!textWidget)
			return false;
		SP_PackageComponent package = SP_PackageComponent.Cast(item.GetOwner().FindComponent(SP_PackageComponent));
		string cname;
		string tname;
		string loc;
		package.GetInfo(cname, tname, loc);
		Name = string.Format("Deliver to %1, located on %2", tname, loc);
		textWidget.SetText(GetItemHintName(item));
		return true;
	}
}
[BaseContainerProps()]
class SCR_ClothItemHintUIInfo : SCR_InventoryItemHintUIInfo
{
	//------------------------------------------------------------------------------------------------
	override bool SetItemHintNameTo(InventoryItemComponent item, TextWidget textWidget)
	{
		SP_ClothAttributes at = SP_ClothAttributes.Cast(item.FindAttribute(SP_ClothAttributes));
		textWidget.SetText(string.Format("Cloth Insulation : %1", at.GetTemperatureIsolation().ToString()));
		return true;
	}
}
[BaseContainerProps()]
class SCR_CanteenInventoryItemHintUIInfo : SCR_InventoryItemHintUIInfo
{
	//------------------------------------------------------------------------------------------------
	override bool SetItemHintNameTo(InventoryItemComponent item, TextWidget textWidget)
	{
		if (!textWidget)
			return false;
		string text;
		SCR_ConsumableItemComponent Canteen = SCR_ConsumableItemComponent.Cast(item.GetOwner().FindComponent(SCR_ConsumableItemComponent));
		int initcap = Canteen.GetInitialCapacity();
		float currentcap = Canteen.GetCurrentCapacity();
		
		if (currentcap == 0)
		{
			text = "Empty";
		}
		else if ( initcap == currentcap)
		{
			text = "Full";
		}
		else if (initcap/2 > currentcap)
		{
			text = "More than half";
		}
		else
		{
			text = "Less than half";
		}
		Name = text;
		textWidget.SetText(GetItemHintName(item));
		return true;
	}
}