//------------------------------------------------------------------//
modded class SCR_GameModeCampaign
{
	[Attribute("INDFORReimagined", category: "Campaign")]
	protected FactionKey m_sINDFOR2FactionKey;
	
	override protected void CheckForWinner()
	{
		return;
	}
	//------------------------------------------------------------------------------------------------
	override FactionKey GetFactionKeyByEnum(SCR_ECampaignFaction faction)
	{
		switch (faction)
		{
			case SCR_ECampaignFaction.INDFOR:
			{
				return m_sINDFORFactionKey;
			};

			case SCR_ECampaignFaction.BLUFOR:
			{
				return m_sBLUFORFactionKey;
			};

			case SCR_ECampaignFaction.OPFOR:
			{
				return m_sOPFORFactionKey;
			};
			case SCR_ECampaignFaction.INDFOR2:
			{
				return m_sINDFOR2FactionKey;
			};
		}

		return FactionKey.Empty;
	}
};
modded enum SCR_ECampaignFaction
{
	INDFOR2,
}