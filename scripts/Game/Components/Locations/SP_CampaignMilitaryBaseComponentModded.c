modded class SCR_CampaignMilitaryBaseComponent
{
	protected IEntity m_eBaseCommander;
	protected IEntity m_eBaseMedic;
	protected IEntity m_eBaseMechanic;
	protected IEntity m_eBaseNavigator;
	protected ref array <IEntity> a_BaseGuards = {};
	
	[Attribute()]
	bool m_bVisited;
	
	protected ref map <Faction, SCR_ECampaignHQRadioComms > m_eFactionRadioCoverage;
	
	void RegisterCharacter(IEntity Char, SCR_ECharacterRank position)
	{
		switch (position)
		{
			case SCR_ECharacterRank.MEDIC:
			{
				m_eBaseMedic = Char;
				return;
			}
			break;
			case SCR_ECharacterRank.MECHANIC:
			{
				m_eBaseMechanic = Char;
				return;
			}
			break;
			case SCR_ECharacterRank.COMMANDER:
			{
				m_eBaseCommander = Char;
				return;
			}
			break;
			case SCR_ECharacterRank.NAVIGATOR:
			{
				m_eBaseNavigator = Char;
				return;
			}
			break;
		}
		a_BaseGuards.Insert(Char);
	}
	void GetCharacterOfPost(SCR_ECharacterRank position, out IEntity Char)
	{
		switch (position)
		{
			case SCR_ECharacterRank.MEDIC:
			{
				Char = m_eBaseMedic;
				return;
			}
			break;
			case SCR_ECharacterRank.MECHANIC:
			{
				Char = m_eBaseMechanic;
				return;
			}
			break;
			case SCR_ECharacterRank.COMMANDER:
			{
				Char = m_eBaseCommander;
				return;
			}
			break;
			case SCR_ECharacterRank.NAVIGATOR:
			{
				Char = m_eBaseNavigator;
				return;
			}
			break;
		}
		if (!a_BaseGuards.IsEmpty())
		{
			Char = a_BaseGuards.GetRandomElement();
		}
	}
	void GetGuards(out array <IEntity> BaseGuards)
	{
		BaseGuards = a_BaseGuards;
	}
	
}