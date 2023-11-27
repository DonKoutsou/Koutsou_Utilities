//------------------------------------------------------------------//
modded class SCR_GameModeCampaign
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
	protected DS_DialogueComponent m_DialogueComponent;
	protected SP_RequestManagerComponent m_RequestManagerComponent;
	protected SCR_FactionManager m_factionManager;
	//------------------------------------------------------------------//
	DS_DialogueComponent GetDialogueComponent()
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
		m_DialogueComponent = DS_DialogueComponent.Cast(owner.FindComponent(DS_DialogueComponent));
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
		SCR_RespawnSystemComponent respawn = SCR_RespawnSystemComponent.Cast(FindComponent(SCR_RespawnSystemComponent));
		if (respawn)
		{
			SCR_AutoSpawnLogic logic = SCR_AutoSpawnLogic.Cast(respawn.GetSpawnLogic());
			logic.DestroyLoadingPlaceholder();
			logic.OpenStartingScreenUI();
		}
		
	}
	//------------------------------------------------------------------------------------------------
	override protected void OnRankChanged(SCR_ECharacterRank oldRank, SCR_ECharacterRank newRank, notnull IEntity owner, bool silent)
	{
		if (silent)
			return;
		SCR_CampaignFaction faction;
		int playerId;
		if (SCR_EntityHelper.IsPlayer(owner))
		{
			playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(owner);
			faction = SCR_CampaignFaction.Cast(SCR_FactionManager.SGetPlayerFaction(playerId));
		}
		else
		{
			FactionAffiliationComponent AffComp = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));
			faction = SCR_CampaignFaction.Cast(AffComp.GetAffiliatedFaction());
		}

		if (!faction)
			return;

		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());

		if (!factionManager)
			return;

		SCR_RankIDCampaign rank = SCR_RankIDCampaign.Cast(factionManager.GetRankByID(newRank));

		if (!rank)
			return;

		SCR_ERadioMsg radio;

		if (newRank < oldRank && !rank.IsRankRenegade())
			radio = SCR_ERadioMsg.DEMOTION;
		else
			radio = rank.GetRadioMsg();
		if (playerId)
			faction.SendHQMessage(radio, calledID: playerId, public: false, param: newRank)
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