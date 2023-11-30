//------------------------------------------------------------------//
modded class SCR_GameModeCampaign
{
	[Attribute("INDFORReimagined", category: "Campaign")]
	protected FactionKey m_sINDFOR2FactionKey;
	
	
	vector m_flastpos;
	
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
			case SCR_ECampaignFaction.RENEGADE:
			{
				return "RENEGADE";
			};
		}

		return FactionKey.Empty;
	}
	
	
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		SCR_PlayerController cont = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		IEntity char = cont.GetControlledEntity();
		if (!char)
			return;
		vector carpos = char.GetOrigin();
		if (m_flastpos != carpos)
		{
			m_flastpos = carpos;
			return;
		}
			
		AIWorld aiworld = GetGame().GetAIWorld();
		AIBirdManagerComponent bman = AIBirdManagerComponent.Cast(aiworld.FindComponent(AIBirdManagerComponent));
		
		IEntity bird = bman.GetClosestDormantAgent(RandomVector(carpos, 10));
		SCR_Bird scrbird = SCR_Bird.Cast(bird);
		if (scrbird)
			scrbird.ActivateAllBirds(true);
		return;
	}
	vector RandomVector(vector pos, float myrange)
	{
		vector sectorAxis, sectorAxisPoint, sectorOrigin, result;
		float range, angle, wantedMaxLength, resX, resZ;
		bool isWaypointRelated;
		IEntity controlledEntity;
		

		
		sectorOrigin = pos;
		wantedMaxLength = myrange;
		sectorAxis = sectorAxisPoint - sectorOrigin;

		angle = Math.Atan2(sectorAxis[2],sectorAxis[0]);
	
		SCR_Math2D.GetRandomPointInSector(sectorOrigin[0],sectorOrigin[2], angle - range, angle + range, wantedMaxLength, resX, resZ);
		
		result = sectorOrigin;
		result[0] = resX;
		result[2] = resZ;
		return result;
	}
};
modded enum SCR_ECampaignFaction
{
	INDFOR2,
	RENEGADE,
}