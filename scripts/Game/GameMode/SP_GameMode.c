//------------------------------------------------------------------//
class SP_GameModeClass: SCR_BaseGameModeClass
{
};
//------------------------------------------------------------------//
class SP_GameMode : SCR_BaseGameMode
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
		ReqMan.AssignInitTasks(entity);
		//Set GM
		SCR_EditorManagerEntity localEditorManager = SCR_EditorManagerEntity.GetInstance();
		SCR_VotingManagerComponent votingManager = SCR_VotingManagerComponent.GetInstance();
		if (votingManager)
		{
			votingManager.StartVoting(EVotingType.EDITOR_IN, localEditorManager.GetPlayerID());
			votingManager.EndVoting(EVotingType.EDITOR_IN, localEditorManager.GetPlayerID(), EVotingOutcome.FORCE_WIN);
		}
		//set up initial goodwill
		m_factionManager.SetUpPlayerGoodwill(entity);
	}
	override void OnPlayerFactionSet_S(SCR_PlayerFactionAffiliationComponent factionComponent, Faction faction)
	{
		plfact = faction;
	}
};
//------------------------------------------------------------------//
modded enum EGameOverTypes
{
	DEATH = 3000,
}
modded class SCR_ArsenalComponent
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