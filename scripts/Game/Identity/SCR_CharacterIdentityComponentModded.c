modded class SCR_CharacterIdentityComponent
{		
	[Attribute(desc : "Stored archetype to be used instead of template in dialogue component")]
	protected ref SP_DialogueArchetype m_DialogueArchetype;
	
	protected int m_iCharacterRep = Math.RandomInt(10, 100);
	
	SP_DialogueArchetype GetArchetype()
		return m_DialogueArchetype;
	
	bool HasArchetype()
	{
		if (m_DialogueArchetype)
			return true;
		return false;
	}
	void SetCarArch(SP_DialogueArchetype Arch)
	{
		m_DialogueArchetype = Arch;
	}
	void SetCharacterRep(int newrep)
	{
		m_iCharacterRep = newrep;
	}
	int GetRep()
	{
		return m_iCharacterRep;
	}
	void AdjustCharRep(int amount, IEntity madude, string Cause = STRING_EMPTY)
	{
		if (!amount)
			return;
		FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(madude.FindComponent(FactionAffiliationComponent));
		m_iCharacterRep = m_iCharacterRep + amount;
		/*if(m_iCharacterRep <= 0)
		{
			FactionComp.SetAffiliatedFactionByKey("RENEGADE");
			if (EntityUtils.IsPlayer(madude))
				SCR_HintManagerComponent.GetInstance().ShowCustom(Cause + " " + "Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
		}*/
		if (amount < 0)
		{
			if (EntityUtils.IsPlayer(madude))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom(Cause + " " + "Your reputation is worsened.");
			}
		}
		else if (amount > 0)
		{
			if (EntityUtils.IsPlayer(madude))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom(Cause + " " + "Your reputation is improved.");
			}
		}
	}
};