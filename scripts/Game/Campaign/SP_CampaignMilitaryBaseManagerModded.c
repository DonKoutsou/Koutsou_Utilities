modded class SCR_CampaignMilitaryBaseManager
{
	ref array <EntityID> a_VisibleBases = {};
	
	SCR_CampaignMilitaryBaseComponent GetClosestBase (vector loc)
	{
		SCR_CampaignMilitaryBaseComponent Base;
		float dist;
		foreach (SCR_CampaignMilitaryBaseComponent campaignBase : m_aBases)
		{
			if (!dist)
			{
				dist = vector.Distance(loc, campaignBase.GetOwner().GetOrigin());
				Base = campaignBase;
				continue;
			}
			float dist2 = vector.Distance(loc, campaignBase.GetOwner().GetOrigin());
			if (dist2 < dist)
			{
				dist = dist2;
				Base = campaignBase;
			}
		}
		return Base;
	}
	
	SCR_CampaignMilitaryBaseComponent GetNamedBase (string name)
	{
		SCR_CampaignMilitaryBaseComponent Base;
		float dist;
		foreach (SCR_CampaignMilitaryBaseComponent campaignBase : m_aBases)
		{
			if (campaignBase.GetBaseName() == name)
			{
				Base = campaignBase;
				break;
			}
		}
		return Base;
	}
	
	bool IsBaseVisible(SCR_CampaignMilitaryBaseComponent base)
	{
		return a_VisibleBases.Contains(base.GetOwner().GetID());
	}
	
	void SetBaseVisible(SCR_CampaignMilitaryBaseComponent base)
	{
		a_VisibleBases.Insert(base.GetOwner().GetID());
		SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Location Marked on Map \n%1", base.GetBaseName()));
	}
	
}

