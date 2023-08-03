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
	[Attribute("1")]
	int m_iLives;
	//------------------------------------------------------------------//
	protected SP_DialogueComponent m_DialogueComponent;
	protected SP_RequestManagerComponent m_RequestManagerComponent;
	protected SP_FactionManager m_factionManager;
	Faction plfact;
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
	SP_FactionManager GetFactionManager()
	{
		return m_factionManager;
	}
	//------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_DialogueComponent = SP_DialogueComponent.Cast(owner.FindComponent(SP_DialogueComponent));
		m_RequestManagerComponent = SP_RequestManagerComponent.Cast(owner.FindComponent(SP_RequestManagerComponent));
		m_factionManager = SP_FactionManager.Cast(GetGame().GetFactionManager());
	}
	//------------------------------------------------------------------//
	protected override void OnControllableSpawned(IEntity entity)
	{
		super.OnControllableSpawned(entity);
	}
	//------------------------------------------------------------------//
	protected override void OnControllableDestroyed(IEntity entity, IEntity instigator)
	{
		super.OnControllableDestroyed(entity, instigator);
		
	}
	//------------------------------------------------------------------//
	protected override void OnControllableDeleted(IEntity entity)
	{
		super.OnControllableDeleted(entity);
		
	}
	//------------------------------------------------------------------//
	protected override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(entity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(10);
		SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		ReqMan.AssignInitTasks(entity);
	}
	//------------------------------------------------------------------//
	protected override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		m_iLives -= 1;
		if (!m_iLives)
		{
			SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
			if (!manager)
				return;
			
			if (m_bOverrideGameOverType)
				manager.SetMissionEndScreen(m_eOverriddenGameOverType);
			
			manager.Finish();
			return;
		}
		super.OnPlayerKilled(playerId, player, killer);
	}
	override void OnPlayerFactionSet_S(SCR_PlayerFactionAffiliationComponent factionComponent, Faction faction)
	{
		plfact = faction;
	}
	//------------------------------------------------------------------//
	//------------------------------------------------------------------------------------------------
	/*
		When a controllable entity is spawned, this event is raised.
		\param entity Spawned entity that raised this event
	*/
	
	
};
//------------------------------------------------------------------//
modded enum EGameOverTypes
{
	DEATH = 3000,
	BROTHERFOUND = 3001,
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