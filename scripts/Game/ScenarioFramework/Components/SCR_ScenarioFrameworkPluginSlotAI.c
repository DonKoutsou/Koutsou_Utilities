[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginSlotAI: SCR_ScenarioFrameworkPlugin
{
	[Attribute(uiwidget: UIWidgets.ResourceAssignArray, desc: "Entities in group non-ai included", params: "et", category: "Group Members")]
	ref array<ResourceName> m_aUnitPrefabSlots;
	
	[Attribute(category: "Player settings", params: "1 100 1")]
	protected int m_iMaxMembers;
	
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		if (!object)
			return;

		super.Init(object);
		SCR_AIGroup Group = SCR_AIGroup.Cast(object.GetSpawnedEntity());
		if (Group)
		{
			Group.m_aUnitPrefabSlots.Clear();
			Group.m_aUnitPrefabSlots.Copy(m_aUnitPrefabSlots);
			Group.SetMaxMembers(m_iMaxMembers);
		}
	}
	void UpdatePreview(SCR_ScenarioFrameworkLayerBase object)
	{
		SCR_ScenarioFrameworkSlotBase slot = SCR_ScenarioFrameworkSlotBase.Cast(object);
		if (!slot)
			return;
		if (!slot.m_PreviewEntity)
			return;
		SCR_AIGroup Group = SCR_AIGroup.Cast(slot.m_PreviewEntity);
		if (Group)
		{
			Group.m_aUnitPrefabSlots.Clear();
			Group.m_aUnitPrefabSlots.Copy(m_aUnitPrefabSlots);
		}
	}
	override void OnWBKeyChanged(SCR_ScenarioFrameworkLayerBase object) 
	{
		super.OnWBKeyChanged(object);
		UpdatePreview(object);
	}
}