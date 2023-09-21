[BaseContainerProps()]
modded class SCR_ItemAttributeCollection
{
	string GetInsulation()
	{
		SP_ClothAttributes at = SP_ClothAttributes.Cast(FindAttribute(SP_ClothAttributes));
		if (!at)
		{
			return STRING_EMPTY;
		}
		string Insulation = "Insulation = " + at.GetTemperatureIsolation().ToString();
		return Insulation;
	};
}
