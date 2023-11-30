[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginSlotAI: SCR_ScenarioFrameworkPlugin
{
	[Attribute(uiwidget: UIWidgets.ResourceAssignArray, desc: "Entities in group non-ai included", params: "et", category: "Group Members")]
	ref array<ResourceName> m_aUnitPrefabSlots;
	
	[Attribute()]
	string m_sFirstNameOverride;
	
	[Attribute()]
	string m_sMiddleNameOverride;
	
	[Attribute()]
	string m_sLastNameOverride;
	
	[Attribute(defvalue : "-1", category: "Player settings", params: "-1 100 1")]
	protected int m_iMaxMembers;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.ComboBox, desc: "Rank", enums: ParamEnumArray.FromEnum(SCR_ECharacterRank))]
	protected SCR_ECharacterRank m_iRank;
	
	[Attribute(defvalue : "0")]
	bool m_bSpawnUncon;
	
	[Attribute("0")]
	bool m_bSetCharacterImportant;
	
	[Attribute(desc : "Stored archetype to be used instead of template in dialogue component")]
	protected ref DS_DialogueArchetype m_DialogueArchetype;
	
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
			{
				array <ref SP_Task> tasks = {};
				SP_RequestManagerComponent.GetCharOwnedTasks(char, tasks);
				if (!tasks.IsEmpty())
				{
					foreach (SP_Task task : tasks)
					{
						SP_ChainedTask chained = SP_ChainedTask.Cast(task);
						if (!chained)
						{
							task.CancelTask();
						}
					}
				}
				SP_RequestManagerComponent.GetInstance().GetCharacterHolder().SetCharImportant(char);
				char.IsImportantCharacter = true;
			}
				
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
			if (m_sFirstNameOverride || m_sMiddleNameOverride || m_sLastNameOverride || m_DialogueArchetype)
			{
				SCR_CharacterIdentityComponent IDComp = SCR_CharacterIdentityComponent.Cast(char.FindComponent(SCR_CharacterIdentityComponent));
				Identity ID = IDComp.GetIdentity();
				if (m_sFirstNameOverride)
				{
					ID.SetName(m_sFirstNameOverride);	
				}
				if (m_sMiddleNameOverride)
				{
					ID.SetAlias(m_sMiddleNameOverride);
				}
				if (m_sLastNameOverride)
				{
					ID.SetSurname(m_sLastNameOverride);
				}
				if (m_DialogueArchetype)
				{
					IDComp.SetCarArch(m_DialogueArchetype);
				}
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
[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginSlotVehicle: SCR_ScenarioFrameworkPlugin
{
	[Attribute(defvalue: "2")]
	int m_iAmountofHZtoDamage;
	
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		SCR_VehicleDamageManagerComponent dmgman = SCR_VehicleDamageManagerComponent.Cast(object.GetSpawnedEntity().FindComponent(SCR_VehicleDamageManagerComponent));
		if (dmgman)
		{
			array<HitZone> hitzones = {};
			dmgman.GetAllHitZones(hitzones);
			if (hitzones.Count() > 0)
			{
				for (int i; i < m_iAmountofHZtoDamage; i++)
				{
					HitZone randhit = hitzones.GetRandomElement();
					randhit.SetHealth(0);
				}
			}
		}
	}
}
[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginSlotDamageHeliRotor: SCR_ScenarioFrameworkPlugin
{
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		SCR_HelicopterDamageManagerComponent Helidamage = SCR_HelicopterDamageManagerComponent.Cast(object.GetSpawnedEntity().FindComponent(SCR_HelicopterDamageManagerComponent));
		SlotManagerComponent Slots = SlotManagerComponent.Cast(object.GetSpawnedEntity().FindComponent(SlotManagerComponent));
		IEntity rotor = Slots.GetSlotByName("RotorMain").GetAttachedEntity();
		SCR_RotorDamageManagerComponent rotordamage = SCR_RotorDamageManagerComponent.Cast(rotor.FindComponent(SCR_RotorDamageManagerComponent));
		SCR_RotorHitZone rotorHitZone = SCR_RotorHitZone.Cast(rotordamage.GetDefaultHitZone());
		rotorHitZone.SetHealth(0);
	}
}
[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_ScenarioFrameworkPluginSlotGrenade: SCR_ScenarioFrameworkPlugin
{
	override void Init(SCR_ScenarioFrameworkLayerBase object)
	{
		TimerTriggerComponent trigger = TimerTriggerComponent.Cast(object.GetSpawnedEntity().FindComponent(TimerTriggerComponent));
		trigger.SetLive();
		WeaponComponent wp = WeaponComponent.Cast(object.GetSpawnedEntity().FindComponent(WeaponComponent));
		wp.ActivateAttachments(true);
	}
}


