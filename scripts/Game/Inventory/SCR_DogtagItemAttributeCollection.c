class SCR_DogtagItemAttributeCollection: SCR_ItemAttributeCollection
{
	private string CharName;
	//------------------------------------------------------------------------------------------------
	string GetCharacterName()
	{
		return CharName;
	};
	void SetCharacterName(string Name)
	{
		CharName = Name;
	}
};
