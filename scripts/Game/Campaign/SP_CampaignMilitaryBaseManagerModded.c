
modded class SCR_CampaignMilitaryBaseManager
{
	ref array <EntityID> a_VisibleBases = {};
	
	
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

