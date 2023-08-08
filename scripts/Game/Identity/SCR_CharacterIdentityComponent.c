[ComponentEditorProps(category: "GameScripted/Identity", description: "")]
class SCR_CharacterIdentityComponentClass : CharacterIdentityComponentClass
{
};

class SCR_CharacterIdentityComponent : CharacterIdentityComponent
{		
	[Attribute("#AR-Idenity_Name_Format_Full")]
	protected LocalizedString m_IdentityFormatFull;
	
	[Attribute("#AR-Idenity_Name_Format_NoAlias")]
	protected LocalizedString m_IdentityFormatNoAlias;
	
	protected int m_iCharacterRep = Math.RandomInt(20, 100);
	
	/*!
	Get full name with formatting. Alias might be empty but it is handeled in formatting
	\param[out] format Either includes or leaves out Alias
	\param[out] name First name of character (%1 in formatting)
	\param[out] alias Alias of character (%2 in formatting) might be empty
	\param[out] surname Surname of Character (%3 in formatting)
	*/
	void SetCharacterRep(int newrep)
	{
		m_iCharacterRep = newrep;
	}
	int GetRep()
	{
		return m_iCharacterRep;
	}
	void AdjustCharRep(int amount, IEntity madude)
	{
		if (!amount)
			return;
		FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(madude.FindComponent(FactionAffiliationComponent));
		m_iCharacterRep = m_iCharacterRep + amount;
		if(m_iCharacterRep <= 0)
		{
			FactionComp.SetAffiliatedFactionByKey("RENEGADE");
			if (EntityUtils.IsPlayer(madude))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
			}
		}
		else
		{
			if (EntityUtils.IsPlayer(madude))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation is worsened");
			}
		}
	}
	void GetFormattedFullName(out string format, out string name, out string alias, out string surname)
	{
		name = GetIdentity().GetName();
		alias = GetIdentity().GetAlias();
		surname = GetIdentity().GetSurname();
		
		if (alias.IsEmpty())
			format = m_IdentityFormatNoAlias;
		else 
			format = m_IdentityFormatFull;
	}
	
	/*!
	Get full name with formatting. Alias might be empty but it is handeled in formatting
	\param[out] format Either includes or leaves out Alias
	\param[out] names Will have an entry for name, alias and surname. Alias can be an empty string
	*/
	void GetFormattedFullName(out string format, out notnull array<string> names)
	{		
		names.Clear();
		
		string name, alias, surname;
		GetFormattedFullName(format, name, alias, surname);
		
		names.Insert(name);
		names.Insert(alias);
		names.Insert(surname);
	}
};