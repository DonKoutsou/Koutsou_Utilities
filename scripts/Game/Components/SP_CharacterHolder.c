//------------------------------------------------------------------//
//Class to contain all characters
class CharacterHolder : ScriptAndConfig
{
	static private ref array <ChimeraCharacter> m_aAliveCharacters;
	static private ref array <ChimeraCharacter> m_aDeadCharacters;

	static ref array<ChimeraCharacter> m_aImportantCharacters;
	//------------------------------------------------------------------------------------------------------------//
	//Name list, holding entity names of important story characters. Will be replaced somehow
	/*void SetSpecialCharacterNameList(array<string> SpecialCharacterNameList)
	{
		if (!m_aSpecialCharacterNameList)
		{
			m_aSpecialCharacterNameList = {};
		}
		m_aSpecialCharacterNameList.Copy(SpecialCharacterNameList);
	}*/
	//------------------------------------------------------------------------------------------------------------//
	//fills provided array with all alive characters
	void GetAllAlive(out array <ChimeraCharacter> AliveCharacters)
	{
		AliveCharacters.Copy(m_aAliveCharacters);
	}
	static void SetCharImportant(IEntity Char)
	{
		SCR_ChimeraCharacter ChimeraChar = SCR_ChimeraCharacter.Cast(Char);
		if (m_aAliveCharacters.Contains(ChimeraChar))
		{
			m_aAliveCharacters.RemoveItem(ChimeraChar);
			m_aImportantCharacters.Insert(ChimeraChar);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//When a new character is spawned check if he is not "special" kekw, and if not inster him in alive array
	private event void OnNewCharacter(IEntity Char)
	{
		SCR_ChimeraCharacter ChimeraChar = SCR_ChimeraCharacter.Cast(Char);
		if (!ChimeraChar)
			return;
		if (ChimeraChar.IsImportantCharacter)
		{
			m_aImportantCharacters.Insert(ChimeraChar);
		}
		m_aAliveCharacters.Insert(ChimeraChar);
	}
	//------------------------------------------------------------------------------------------------------------//
	//When a character dies make sure to remove him from alive array and put him in dead
	private event void OnCharacterDeath(IEntity Char)
	{
		ChimeraCharacter ChimeraChar = ChimeraCharacter.Cast(Char);
		if (!ChimeraChar)
			return;
		m_aAliveCharacters.RemoveItem(ChimeraChar);
		m_aDeadCharacters.Insert(ChimeraChar);
	}
	//------------------------------------------------------------------------------------------------------------//
	//when a character is deleted make sure to remove him of any arrays he might still be in
	private event void OnCharacterDeleted(IEntity Char)
	{
		ChimeraCharacter ChimeraChar = ChimeraCharacter.Cast(Char);
		if (!ChimeraChar)
			return;
		if (m_aAliveCharacters.Contains(ChimeraChar))
			m_aAliveCharacters.RemoveItem(ChimeraChar);
		if (m_aDeadCharacters.Contains(ChimeraChar))
			m_aDeadCharacters.RemoveItem(ChimeraChar);
	}
	//------------------------------------------------------------------------------------------------------------//
	//Amount of Alive characters
	static int GetAliveCount()
	{
		return m_aAliveCharacters.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	//Amount of Dead characters
	static int GetDeadCount()
	{
		return m_aDeadCharacters.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	static ChimeraCharacter GetRandomDeadOfFaction(Faction fact)
	{
		ChimeraCharacter mychar;
		if (m_aDeadCharacters.IsEmpty())
			return null;
		for (int i = 0; i < 10; i++)
		{
			mychar = m_aDeadCharacters.GetRandomElement();
			if (!mychar)
				continue;
			
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if (!Aff)
				continue;
			if(Aff.GetAffiliatedFaction() == fact)
			{
				return mychar;
			}
		}
		mychar = null;
		return mychar;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool GetUnitOfFaction(Faction fact, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(mychar))
				continue;
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if (!Aff)
				continue;
			if(Aff.GetAffiliatedFaction() == fact)
			{
				return true;
			}
		}
		mychar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool GetUnitOfAnyFriendlyFaction(Faction fact, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(mychar))
				continue;
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(mychar.FindComponent(FactionAffiliationComponent));
			if (!Aff)
				continue;
			if(Aff.GetAffiliatedFaction().IsFactionFriendly(fact))
			{
				return true;
			}
		}
		mychar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	static bool GetCharOfRank(SCR_CharacterRank rank, out ChimeraCharacter mychar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(mychar))
				continue;
			SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(mychar.FindComponent(SCR_CharacterRankComponent));
			if (!Rankcomp)
				continue;
			if(Rankcomp.GetCharacterRankName(mychar) == rank.ToString())
			{
				return true;
			}
		}
		mychar = null;
		return false;	
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit far form provided characters location
	static bool GetFarUnit(ChimeraCharacter mychar, float mindistance, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(FarChar))
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), mychar.GetOrigin());
			if (mindistance > dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit far form provided location
	static bool GetFarUnit(vector pos, float mindistance, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(FarChar))
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), pos);
			if (mindistance < dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit close provided location
	static bool GetCloseUnit(vector pos, float maxdistance, out ChimeraCharacter CloseChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetRandomUnit(CloseChar))
				continue;
			float dist = vector.Distance(CloseChar.GetOrigin(), pos);
			if (maxdistance > dist)
				return true;
		}
		CloseChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit of specified faction far form provided character
	static bool GetFarUnitOfFaction(ChimeraCharacter mychar, float mindistance, Faction fact, out ChimeraCharacter FarChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetUnitOfFaction(fact, FarChar))
				continue;
			if (FarChar == mychar)
				continue;
			float dist = vector.Distance(FarChar.GetOrigin(), mychar.GetOrigin());
			if (mindistance < dist)
				return true;
		}
		FarChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Get unit of specified faction clsoe to provided character
	static bool GetCloseUnitOfFaction(ChimeraCharacter mychar, float maxdistance, Faction fact, out ChimeraCharacter CloseChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetUnitOfFaction(fact, CloseChar))
				continue;
			if (CloseChar == mychar)
				continue;
			float dist = vector.Distance(CloseChar.GetOrigin(), mychar.GetOrigin());
			if (maxdistance > dist)
				return true;
		}
		CloseChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//
	static bool GetCloseUnitOfFriendlyFaction(ChimeraCharacter mychar, float maxdistance, Faction fact, out ChimeraCharacter CloseChar)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!GetUnitOfAnyFriendlyFaction(fact, CloseChar))
				continue;
			if (CloseChar == mychar)
				continue;
			float dist = vector.Distance(CloseChar.GetOrigin(), mychar.GetOrigin());
			if (maxdistance > dist)
				return true;
		}
		CloseChar = null;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Gives random unit, make sure not to get player
	static bool GetRandomUnit(out ChimeraCharacter mychar)
	{
		if (m_aAliveCharacters.IsEmpty())
			return false;
		mychar = m_aAliveCharacters.GetRandomElement();
		//if (m_aSpecialCharacterNameList.Contains(mychar.GetName()))
			//return false;
		//if (mychar.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup())
			//if (m_aSpecialCharacterNameList.Contains(mychar.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetName()))
				//return false;
		if (SCR_EntityHelper.IsPlayer(mychar))
			return false;
		if (mychar)
			return true;
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	/*bool GetStrandedGroup(out SCR_AIGroup group)
	{
		for (int i = 0; i < 10; i++)
		{
			group = SCR_AIGroup.Cast(m_aAliveCharacters.GetRandomElement().GetParent());
			if (!group)
				continue;
		
			if (CheckForCharacters(200, group.GetOrigin()))
				group = null;
			if (group)
				return true;
		}
		return false;
	}*/
	//------------------------------------------------------------------------------------------------------------//
	static void CharacterHolder()
	{
		Init();
	}
	//------------------------------------------------------------------------------------------------------------//
	static void ~CharacterHolder()
	{
		if (m_aAliveCharacters)
			m_aAliveCharacters.Clear();
		if (m_aDeadCharacters)
			m_aDeadCharacters.Clear();
		if (m_aImportantCharacters)
			m_aImportantCharacters.Clear();
	}
	
	//------------------------------------------------------------------------------------------------------------//
	static void Init()
	{
		if (!m_aAliveCharacters)
			m_aAliveCharacters = new ref array <ChimeraCharacter>();
		if (!m_aDeadCharacters)
			m_aDeadCharacters = new ref array <ChimeraCharacter>();
		if (!m_aImportantCharacters)
			m_aImportantCharacters = new array<ChimeraCharacter>();
	}
	//------------------------------------------------------------------------------------------------------------//
}
