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
	[Attribute("INDFORReimagined", category: "Campaign")]
	protected FactionKey m_sINDFOR2FactionKey;
	
	
	vector m_flastpos;
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
	override protected void CheckForWinner()
	{
		return;
	}
	//------------------------------------------------------------------------------------------------
	override FactionKey GetFactionKeyByEnum(SCR_ECampaignFaction faction)
	{
		switch (faction)
		{
			case SCR_ECampaignFaction.INDFOR:
			{
				return m_sINDFORFactionKey;
			};

			case SCR_ECampaignFaction.BLUFOR:
			{
				return m_sBLUFORFactionKey;
			};

			case SCR_ECampaignFaction.OPFOR:
			{
				return m_sOPFORFactionKey;
			};
			case SCR_ECampaignFaction.INDFOR2:
			{
				return m_sINDFOR2FactionKey;
			};
			case SCR_ECampaignFaction.RENEGADE:
			{
				return "RENEGADE";
			};
		}

		return FactionKey.Empty;
	}
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		SCR_PlayerController cont = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		IEntity char = cont.GetControlledEntity();
		if (!char)
			return;
		vector carpos = char.GetOrigin();
		if (m_flastpos != carpos)
		{
			m_flastpos = carpos;
			return;
		}
			
		AIWorld aiworld = GetGame().GetAIWorld();
		AIBirdManagerComponent bman = AIBirdManagerComponent.Cast(aiworld.FindComponent(AIBirdManagerComponent));
		
		IEntity bird = bman.GetClosestDormantAgent(RandomVector(carpos, 10));
		SCR_Bird scrbird = SCR_Bird.Cast(bird);
		if (scrbird)
			scrbird.ActivateAllBirds(true);
		return;
	}
	vector RandomVector(vector pos, float myrange)
	{
		vector sectorAxis, sectorAxisPoint, sectorOrigin, result;
		float range, angle, wantedMaxLength, resX, resZ;
		bool isWaypointRelated;
		IEntity controlledEntity;
		

		
		sectorOrigin = pos;
		wantedMaxLength = myrange;
		sectorAxis = sectorAxisPoint - sectorOrigin;

		angle = Math.Atan2(sectorAxis[2],sectorAxis[0]);
	
		SCR_Math2D.GetRandomPointInSector(sectorOrigin[0],sectorOrigin[2], angle - range, angle + range, wantedMaxLength, resX, resZ);
		
		result = sectorOrigin;
		result[0] = resX;
		result[2] = resZ;
		return result;
	}
};
//------------------------------------------------------------------//
modded enum EGameOverTypes
{
	DEATH = 3000,
}
modded enum SCR_ECampaignFaction
{
	INDFOR2,
	RENEGADE,
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