//------------------------------------------------------------------//
//Class to contain all characters
class CharacterHolder : ScriptAndConfig
{
	static private ref array <ChimeraCharacter> AliveCharacters;
	static private ref array <ChimeraCharacter> DeadCharacters;

	static ref array<string> m_aSpecialCars;
	
	//------------------------------------------------------------------------------------------------------------//
	array <ChimeraCharacter> GetAllAlive()
	{
		return AliveCharacters;
	}
	//------------------------------------------------------------------------------------------------------------//
	static void InserCharacter(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		if (m_aSpecialCars.Contains(Char.GetName()))
			return;
		AliveCharacters.Insert(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	static void CharIsDead(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		AliveCharacters.RemoveItem(Char);
		DeadCharacters.Insert(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	static void CharIsDel(ChimeraCharacter Char)
	{
		if (!Char)
			return;
		if (AliveCharacters.Contains(Char))
			AliveCharacters.RemoveItem(Char);
		if (DeadCharacters.Contains(Char))
			DeadCharacters.RemoveItem(Char);
	}
	//------------------------------------------------------------------------------------------------------------//
	static int GetAliveCount()
	{
		return AliveCharacters.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	static ChimeraCharacter GetRandomDeadOfFaction(Faction fact)
	{
		ChimeraCharacter mychar;
		if (DeadCharacters.IsEmpty())
			return null;
		for (int i = 0; i < 10; i++)
		{
			mychar = DeadCharacters.GetRandomElement();
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
	static bool GetRandomUnit(out ChimeraCharacter mychar)
	{
		if (AliveCharacters.IsEmpty())
			return false;
		mychar = AliveCharacters.GetRandomElement();
		if (m_aSpecialCars.Contains(mychar.GetName()))
			return false;
		if (mychar.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup())
			if (m_aSpecialCars.Contains(mychar.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetName()))
				return false;
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
			group = SCR_AIGroup.Cast(AliveCharacters.GetRandomElement().GetParent());
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
	void CharacterHolder()
	{
		if (!AliveCharacters)
			AliveCharacters = new ref array <ChimeraCharacter>();
		if (!DeadCharacters)
			DeadCharacters = new ref array <ChimeraCharacter>();
		if (!m_aSpecialCars)
			m_aSpecialCars = new array<string>();
	}
	//------------------------------------------------------------------------------------------------------------//
	void ~CharacterHolder()
	{
		AliveCharacters.Clear();
		DeadCharacters.Clear();
		m_aSpecialCars.Clear();
	}
	//------------------------------------------------------------------------------------------------------------//
	void Init();
	//------------------------------------------------------------------------------------------------------------//
}