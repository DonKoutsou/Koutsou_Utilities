[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginSlotAI: SCR_ScenarioFrameworkPlugin
{
	[Attribute(uiwidget: UIWidgets.ResourceAssignArray, desc: "Entities in group non-ai included", params: "et", category: "Group Members")]
	ref array<ResourceName> m_aUnitPrefabSlots;
	
	[Attribute(defvalue : "-1", category: "Player settings", params: "-1 100 1")]
	protected int m_iMaxMembers;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.ComboBox, desc: "Rank", enums: ParamEnumArray.FromEnum(SCR_ECharacterRank))]
	protected SCR_ECharacterRank m_iRank;
	
	[Attribute(defvalue : "0")]
	bool m_bSpawnUncon;
	
	[Attribute("0")]
	bool m_bSetCharacterImportant;
	
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		if (!object)
			return;

		super.Init(object);
		SCR_AIGroup Group = SCR_AIGroup.Cast(object.GetSpawnedEntity());
		if (Group)
		{
			if (m_aUnitPrefabSlots.Count() > 0)
			{
				Group.m_aUnitPrefabSlots.Clear();
				Group.m_aUnitPrefabSlots.Copy(m_aUnitPrefabSlots);
			}
			if (m_iMaxMembers != -1)
				Group.SetMaxMembers(m_iMaxMembers);
			if (m_bSpawnUncon)
			{
				array<AIAgent> outAgents = new array<AIAgent>();
				Group.GetAgents(outAgents);
				foreach(AIAgent agent : outAgents)
				{
					IEntity Char = agent.GetControlledEntity();
					if(Char)
					{
						SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
						if(dmg.GetIsUnconscious())
						{
							dmg.SetResilienceRegenScale(0);
							continue;
						}
						dmg.ForceUnconsciousness();
						dmg.SetResilienceRegenScale(0);
					}
				}
			}
		}
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(object.GetSpawnedEntity());
		if (char)
		{
			if (m_bSetCharacterImportant)
				char.IsImportantCharacter = true;
			if (m_bSpawnUncon)
			{
				SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(char.FindComponent(SCR_CharacterDamageManagerComponent));
				if(dmg.GetIsUnconscious())
				{
					return;
				}
				dmg.ForceUnconsciousness();
				dmg.SetResilienceRegenScale(0);
			}
			if (m_iRank != SCR_ECharacterRank.INVALID)
			{
				SCR_CharacterRankComponent rankcomp = SCR_CharacterRankComponent.Cast(char.FindComponent(SCR_CharacterRankComponent));
				rankcomp.SetCharacterRank(m_iRank);
			}
		}
	}
}
//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_ContainerActionTitle()]
class SP_ScenarioFrameworkActionBleedChar : SCR_ScenarioFrameworkActionBase
{
	[Attribute()];
	protected string m_sCharName;

			
	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity object)
	{
		if (!CanActivate())
			return;
		
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(GetGame().FindEntity(m_sCharName));
		if (Char)
		{
			SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
			dmg.AddParticularBleeding();
		}
	}
}

