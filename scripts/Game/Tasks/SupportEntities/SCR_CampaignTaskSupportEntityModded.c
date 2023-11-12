modded class SCR_CampaignTaskSupportEntity
{
	override void GenerateCaptureTasks(notnull IEntity entity)
	{
		return;
		if (!GetTaskManager())
			return;
		
		SCR_CampaignTaskType type = SCR_CampaignTaskType.CAPTURE;
		array<SCR_CampaignMilitaryBaseComponent> bases = new array<SCR_CampaignMilitaryBaseComponent>();
		SCR_CampaignFaction faction;
		SCR_CampaignMilitaryBaseComponent capturedBase = SCR_CampaignMilitaryBaseComponent.Cast(entity.FindComponent(SCR_CampaignMilitaryBaseComponent));
		
		if (capturedBase)
		{
			// It's a base
			m_aCheckedBases.Insert(capturedBase);
			GenerateRecaptureTask(capturedBase);
			faction = capturedBase.GetCampaignFaction();
			
			if (!faction)
				return;
			
			if (!capturedBase.IsHQRadioTrafficPossible(faction))
				return;
			
			capturedBase.GetBasesInRange(SCR_ECampaignBaseType.BASE | SCR_ECampaignBaseType.RELAY, bases);
			
			foreach (SCR_CampaignMilitaryBaseComponent base: bases)
			{
				if (base.IsHQ())
					continue;
				
				if (GetTask(base, faction, type))
					continue;
				
				if (base.GetFaction() == faction)
				{
					if (m_aCheckedBases.Contains(base))
						continue;
					
					GenerateCaptureTasks(base.GetOwner());
					continue;
				}
				
				CreateNewCampaignTask(type, base, faction, true);
			}
		}
		else
		{
			// It's a mobile HQ
			SCR_CampaignMobileAssemblyComponent comp = SCR_CampaignMobileAssemblyComponent.Cast(entity.FindComponent(SCR_CampaignMobileAssemblyComponent));
			
			if (!comp)
				return;
			
			faction = comp.GetParentFaction();
			comp.GetBasesInRange(bases);
			
			foreach (SCR_CampaignMilitaryBaseComponent base: bases)
			{
				if (GetTask(base, faction, type))
					continue;
				
				if (base.IsHQ())
					continue;
				
				if (base.GetFaction() == faction)
				{
					if (m_aCheckedBases.Contains(base))
						continue;
					
					GenerateCaptureTasks(base.GetOwner());
					continue;
				}
				
				CreateNewCampaignTask(type, base, faction, true);
			}
		}
	}
};