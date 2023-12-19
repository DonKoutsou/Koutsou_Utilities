modded class SCR_AmbientPatrolManager
{
	override void SetFactionOfSpawnpoint(int spawnpointIndex)
	{
		bool factionupdated;
		SCR_AmbientPatrolSpawnPointComponent spawnpoint = m_aPatrols[spawnpointIndex];
		FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(spawnpoint.GetOwner().FindComponent(FactionAffiliationComponent));
		SCR_GameModeCampaign Camp = SCR_GameModeCampaign.GetInstance();
		SCR_CampaignMilitaryBaseManager CampMan = Camp.GetBaseManager();
		array<SCR_CampaignMilitaryBaseComponent> bases = {};
		CampMan.GetBases(bases);
		if (bases.IsEmpty())
		{
			return;
		}
		float dist;
		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.GetFaction())
				continue;
			if (!dist)
			{
				dist = vector.Distance( spawnpoint.GetOwner().GetOrigin(), base.GetOwner().GetOrigin());
				affcomp.SetAffiliatedFactionByKey( base.GetFaction().GetFactionKey() );
			}
			
			float dist2 = vector.Distance( spawnpoint.GetOwner().GetOrigin(), base.GetOwner().GetOrigin());
			if ( dist2 < dist)
			{
				dist = dist2;
				affcomp.SetAffiliatedFactionByKey( base.GetFaction().GetFactionKey() );
			}
			if (dist < base.m_iBasePatrolInfluance)
			{
				factionupdated = true;
			}
		}
		if (!factionupdated)
		{
			 int rand = Math.RandomInt(0,2);
			if (!rand)
				affcomp.SetAffiliatedFactionByKey( Camp.GetFactionByEnum(SCR_ECampaignFaction.BANDITS).GetFactionKey() );
			else
				affcomp.SetAffiliatedFactionByKey( Camp.GetFactionByEnum(SCR_ECampaignFaction.RENEGADE).GetFactionKey() );
		}
	}
}