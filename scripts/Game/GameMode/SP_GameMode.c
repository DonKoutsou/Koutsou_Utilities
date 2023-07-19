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
	protected override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(controlledEntity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(10);
	}
	//------------------------------------------------------------------//
	protected override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		super.OnPlayerKilled(playerId, player, killer);
		
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
