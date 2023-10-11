//------------------------------------------------------------------//
class SP_GameModeClass: SCR_GameModeCampaignClass
{
};
//------------------------------------------------------------------//
class SP_GameMode : SCR_GameModeCampaign
{
	//------------------------------------------------------------------//
	[Attribute(UIWidgets.CheckBox, desc: "If true, it will override any previously set game over type with selected one down bellow")];
	protected bool		m_bOverrideGameOverType;
	//------------------------------------------------------------------//
	[Attribute("1", UIWidgets.ComboBox, "Game Over Type", "", ParamEnumArray.FromEnum(EGameOverTypes))];
	protected EGameOverTypes			m_eOverriddenGameOverType;
	//------------------------------------------------------------------//
	[Attribute(defvalue : "40")]
	protected int m_iPlayerInitialRep;
	protected SP_DialogueComponent m_DialogueComponent;
	protected SP_RequestManagerComponent m_RequestManagerComponent;
	protected SCR_FactionManager m_factionManager;
	protected Faction plfact;
	
	Faction GetPlFaction()
	{
		return plfact;
	}
	//------------------------------------------------------------------//
	SP_DialogueComponent GetDialogueComponent()
	{
		return m_DialogueComponent;
	}
	//------------------------------------------------------------------//
	SP_RequestManagerComponent GetRequestManagerComponent()
	{
		return m_RequestManagerComponent;
	}
	//------------------------------------------------------------------//
	SCR_FactionManager GetFactionManager()
	{
		return m_factionManager;
	}
	//------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_DialogueComponent = SP_DialogueComponent.Cast(owner.FindComponent(SP_DialogueComponent));
		m_RequestManagerComponent = SP_RequestManagerComponent.Cast(owner.FindComponent(SP_RequestManagerComponent));
		m_factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
	}
	//------------------------------------------------------------------//
	protected override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
		//set initial rep
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(entity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(m_iPlayerInitialRep);
		//Assign questlines
		SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if (ReqMan)
			ReqMan.AssignInitTasks(entity);
		//set up initial goodwill
		m_factionManager.SetUpPlayerGoodwill(entity);
		
		array <ref SP_Task> tasks = {};
		SP_RequestManagerComponent.GetCharTasks(entity, tasks);
		foreach (SP_Task task : tasks)
		{
			task.CancelTask();
		}
	}
	override void OnPlayerFactionSet_S(SCR_PlayerFactionAffiliationComponent factionComponent, Faction faction)
	{
		plfact = faction;
	}
	override bool RplLoad(ScriptBitReader reader)
	{
		// Sync respawn radios & control points amount
		int activeBasesTotal;

		reader.ReadInt(activeBasesTotal);

		m_BaseManager.SetTargetActiveBasesCount(activeBasesTotal);

		if (m_BaseManager.GetActiveBasesCount() == activeBasesTotal)
			m_BaseManager.OnAllBasesInitialized();

		int respawnRadiosBLUFOR, respawnRadiosOPFOR, respawnRadiosINFOR, controlPointsHeldBLUFOR, controlPointsHeldOPFOR, controlPointsHeldINFOR, primaryTargetBLUFOR, primaryTargetOPFOR, primaryTargetINFOR;

		reader.ReadInt(respawnRadiosBLUFOR);
		reader.ReadInt(respawnRadiosOPFOR);
		reader.ReadInt(respawnRadiosINFOR);

		reader.ReadInt(controlPointsHeldBLUFOR);
		reader.ReadInt(controlPointsHeldOPFOR);
		reader.ReadInt(controlPointsHeldINFOR);

		reader.ReadInt(primaryTargetBLUFOR);
		reader.ReadInt(primaryTargetOPFOR);
		reader.ReadInt(primaryTargetINFOR);

		GetFactionByEnum(SCR_ECampaignFaction.BLUFOR).SetActiveRespawnRadios(respawnRadiosBLUFOR);
		GetFactionByEnum(SCR_ECampaignFaction.OPFOR).SetActiveRespawnRadios(respawnRadiosOPFOR);
		GetFactionByEnum(SCR_ECampaignFaction.OPFOR).SetActiveRespawnRadios(respawnRadiosINFOR);

		GetFactionByEnum(SCR_ECampaignFaction.BLUFOR).SetControlPointsHeld(controlPointsHeldBLUFOR);
		GetFactionByEnum(SCR_ECampaignFaction.OPFOR).SetControlPointsHeld(controlPointsHeldOPFOR);
		GetFactionByEnum(SCR_ECampaignFaction.OPFOR).SetControlPointsHeld(controlPointsHeldINFOR);

		GetFactionByEnum(SCR_ECampaignFaction.BLUFOR).SetPrimaryTarget(SCR_CampaignMilitaryBaseComponent.Cast(Replication.FindItem(primaryTargetBLUFOR)));
		GetFactionByEnum(SCR_ECampaignFaction.OPFOR).SetPrimaryTarget(SCR_CampaignMilitaryBaseComponent.Cast(Replication.FindItem(primaryTargetOPFOR)));

		return true;
	}
};
//------------------------------------------------------------------//
modded enum EGameOverTypes
{
	DEATH = 3000,
}
/*modded class SCR_ArsenalComponent
{
	override protected void OnPostInit(IEntity owner)
	{
		Deactivate(owner);
	}
	override protected void EOnInit(IEntity owner)
	{
		Deactivate(owner);
	}
}
modded class SCR_ArsenalDisplayComponent
{
	override void EOnInit(IEntity owner)
	{
		Deactivate(owner);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		Deactivate(owner);
	}
}