//------------------------------------------------------------------------------------------------
/*modded class SCR_SupplyInventorySlotUI
{
	const ResourceName medicalicon = "{20B1D468F39B0F82}UI/Textures/Editor/ContentBrowser/ContentBrowser_Trait_Medical.edds";
	const ResourceName repairicon = "{203F5DF31C1A45FB}UI/Textures/Editor/ContentBrowser/ContentBrowser_Trait_Repairing.edds";
	override void UpdateConsumer()
	{
		if (!m_ResourceComponent)
			return;
		
		float totalResources, maxResources;
		
		array<SCR_ResourceContainer> cont = m_ResourceComponent.GetContainers();
		if (cont.Count() == 1)
		{
			m_eResourceType = cont[0].GetResourceType();
		}
		if (m_eResourceType == EResourceType.Medical)
		{
			ImageWidget image = ImageWidget.Cast(m_widget.FindAnyWidget("SuppliesIcon2"));
			if (image)
				image.LoadImageTexture(0, medicalicon);
		}
		if (m_eResourceType == EResourceType.Repair)
		{
			ImageWidget image = ImageWidget.Cast(m_widget.FindAnyWidget("SuppliesIcon2"));
			if (image)
				image.LoadImageTexture(0, repairicon);
		}
		//~ Stored supplies
		bool showUI = SCR_ResourceSystemHelper.GetMaxAndStoredResources(m_ResourceComponent, totalResources, maxResources, m_eResourceType);
		UpdateStoredResources(showUI, totalResources, maxResources);
		
		//~ Available Supplies
		showUI = SCR_ResourceSystemHelper.GetAvailableResources(m_ResourceComponent, totalResources, m_eResourceType);
		UpdateAvailableResources(showUI, totalResources);
	}

};