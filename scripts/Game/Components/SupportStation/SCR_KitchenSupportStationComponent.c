[ComponentEditorProps(category: "GameScripted/SupportStation", description: "")]
class SCR_KitchenSupportStationComponentClass : SCR_BaseSupportStationComponentClass
{	
};

class SCR_KitchenSupportStationComponent : SCR_BaseSupportStationComponent
{
	override ESupportStationType GetSupportStationType()
	{
		return ESupportStationType.FOOD;
	}
};
modded enum ESupportStationType
{
	FOOD,
};