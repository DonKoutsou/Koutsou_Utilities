[BaseContainerProps(configRoot:true)]
class SP_DialogueStageBaseTaskAction : DS_BaseDialogueStageAction
{
	[Attribute(defvalue: "1")]
	bool m_bGetOwnedTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetTargetTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetAssignedTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetReadyToDeliver;
	
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskType))]
	int m_iGetTasksOfType;
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskState))]
	int m_iGetTasksWithState;
	[Attribute(desc: "Show player tasks")]
	bool m_bUsePlayer;
	override void Perform(IEntity Character, IEntity Player)
	{
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageAssignTaskAction : SP_DialogueStageBaseTaskAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		}
		SP_RequestManagerComponent Reqman = SP_RequestManagerComponent.GetInstance();
		array <ref SP_Task> tasklist = {};
		if (m_bGetOwnedTasks)
			Reqman.GetCharOwnedTasks(CharToCheck, tasklist);
		if (m_bGetTargetTasks)
			Reqman.GetCharTargetTasks(CharToCheck, tasklist);
		if (m_bGetAssignedTasks)
			Reqman.GetassignedTasks(CharToCheck, tasklist);
		if (m_bGetReadyToDeliver)
			Reqman.GetReadyToDeliver(CharToCheck, tasklist, CharToAssign);
		if (m_iGetTasksOfType)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetTaskType() != m_iGetTasksOfType)
					tasklist.Remove(i);
			}
		}
		if (m_iGetTasksWithState)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetState() != m_iGetTasksWithState)
					tasklist.Remove(i);
			}
		}
		
		if (m_iIndex >= tasklist.Count())
		{
			return;
		}
		SP_Task t_Task = tasklist.Get(m_iIndex);
		t_Task.AssignCharacter(CharToAssign);
		super.Perform(Character, Player);
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageCompleteTaskAction : SP_DialogueStageBaseTaskAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		};
		SP_RequestManagerComponent Reqman = SP_RequestManagerComponent.GetInstance();
		array <ref SP_Task> tasklist = {};
		if (m_bGetOwnedTasks)
			Reqman.GetCharOwnedTasks(CharToCheck, tasklist);
		if (m_bGetTargetTasks)
			Reqman.GetCharTargetTasks(CharToCheck, tasklist);
		if (m_bGetAssignedTasks)
			Reqman.GetassignedTasks(CharToCheck, tasklist);
		if (m_bGetReadyToDeliver)
			Reqman.GetReadyToDeliver(CharToCheck, tasklist, CharToAssign);
		if (m_iGetTasksOfType)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetTaskType() != m_iGetTasksOfType)
					tasklist.Remove(i);
			}
		}
		if (m_iGetTasksWithState)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetState() != m_iGetTasksWithState)
					tasklist.Remove(i);
			}
		}
		if (m_iIndex >= tasklist.Count())
		{
			return;
		}
		SP_Task t_Task = tasklist.Get(m_iIndex);
		t_Task.CompleteTask(CharToAssign);
		super.Perform(Character, Player);
	};
};
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageCreateTaskAction : DS_BaseDialogueStageAction
{
	[Attribute()]
	ref SP_Task m_Task;
	
	[Attribute()]
	bool m_bSetCharAsOwner;
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if (m_bSetCharAsOwner)
			ReqMan.CreateCustomTaskOfType(m_Task.GetClassName(), Character);
		else
			ReqMan.CreateTask(m_Task.GetClassName());
	};
};
[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class SP_DialogueStageAssignBaseWPAction : DS_BaseDialogueStageAction
{
	[Attribute(defvalue: "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, desc: "Default waypoint prefab", "et", category: "Defender Spawner")]
	ResourceName m_sDefaultWaypointPrefab;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		vector spawnpos;
		Resource wpRes;
		SCR_AIWaypoint wp;
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		SCR_GameModeCampaign gamemod = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		SCR_CampaignMilitaryBaseManager Baseman = gamemod.GetBaseManager();
		vector pos = Character.GetOrigin();
		SCR_CampaignMilitaryBaseComponent base = Baseman.FindClosestBase(pos);
		if (!base)
			return;
		base.GetOwner().GetTransform(params.Transform);
		
		wpRes = Resource.Load(m_sDefaultWaypointPrefab);
		if (!wpRes.IsValid())
			return;
		wp = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefabLocal(wpRes, null, params));
		if (!wp)
			return;
		ChimeraCharacter char = ChimeraCharacter.Cast(Character);
		
		SCR_AIGroup group = SCR_AIGroup.Cast(char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup());
		
		group.AddWaypoint(wp);
	};
};
[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class SP_DialogueStageStopTalkAction : DS_BaseDialogueStageAction
{
	
	override void Perform(IEntity Character, IEntity Player)
	{
		super.Perform(Character, Player);
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		SCR_AITalkToCharacterBehavior act = SCR_AITalkToCharacterBehavior.Cast(utility.FindActionOfType(SCR_AITalkToCharacterBehavior));
		if (act)
			act.SetActiveConversation(false);

	};
};
[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class SP_DialogueStageLowerWeaponAction : DS_BaseDialogueStageAction
{
	[Attribute()]
	bool m_bRaiseWeapon;
	override void Perform(IEntity Character, IEntity Player)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(Character);
		SCR_CharacterControllerComponent cont = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		SCR_ChimeraAIAgent chimeraAgent = SCR_ChimeraAIAgent.Cast(cont.GetAIControlComponent().GetAIAgent());

		SCR_AIInfoComponent m_AIInfo = chimeraAgent.m_InfoComponent;
		
		m_AIInfo.SetWeaponRaised(m_bRaiseWeapon);
		cont.SetWeaponRaised(m_bRaiseWeapon);

	};
};
[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class SP_DialogueStageMakeBaseVisitedAction : DS_BaseDialogueStageAction
{
	override void Perform(IEntity Character, IEntity Player)
	{
		SCR_CampaignMilitaryBaseManager BaseMan = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
		SCR_CampaignMilitaryBaseComponent nearest = BaseMan.GetClosestBase(Character.GetOrigin());
		if (!nearest)
			return;
		nearest.m_bVisited = true;	

	};
};
[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class SP_DialogueStageReplaceArchetypeAction : DS_BaseDialogueStageAction
{
	[Attribute()]
	ResourceName m_Archetype;
	
	protected ref DS_DialogueArchetype DialogueArchetype;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		DS_DialogueComponent diag = DS_DialogueComponent.GetInstance();
		DS_DialogueArchetype DiagArchNew = diag.CopyArchetype(DialogueArchetype);
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Character.FindComponent(SCR_CharacterIdentityComponent));
		
		id.SetCarArch(DialogueArchetype);
		diag.UnregisterArchtype(Character, DialogueArchetype);
	};
	
	override void PostInit()
	{
		DS_DialogueComponent diag = DS_DialogueComponent.GetInstance();
		Resource arch = Resource.Load(m_Archetype);
		BaseResourceObject archobj = arch.GetResource();
		BaseContainer archcont = archobj.ToBaseContainer();
		DialogueArchetype = DS_DialogueArchetype.Cast(BaseContainerTools.CreateInstanceFromContainer(archcont));
	}
};

class SCR_AISetConverseFalse : SCR_AIActionTask
{

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AIActionBase action = GetExecutedAction();
		
		if (!action)
			return ENodeResult.FAIL;
		SCR_AILeadCharacterOutOfBaseBehavior follow = SCR_AILeadCharacterOutOfBaseBehavior.Cast(action);
		if (follow){follow.SetActiveConversation(false)};
		return ENodeResult.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	protected override string GetOnHoverDescription()
	{
		return "Completes action specified in input or current action";
	}		

};

[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class SP_DialogueStageStartEvacBehAction : DS_BaseDialogueStageAction
{
	
	override void Perform(IEntity Character, IEntity Player)
	{
		super.Perform(Character, Player);
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		
		SCR_CampaignMilitaryBaseManager BaseMan = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
		SCR_CampaignMilitaryBaseComponent nearest = BaseMan.GetClosestBase(Character.GetOrigin());		
		
		SCR_AILeadCharacterOutOfBaseBehavior act = new SCR_AILeadCharacterOutOfBaseBehavior(utility, null, Player, nearest.GetOwner().GetOrigin(), nearest.GetRadius());
		
		utility.AddAction(act);
	};
};