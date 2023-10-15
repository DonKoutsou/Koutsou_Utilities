[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageFollowAction : DialogueStage
{
	[Attribute()]
	ResourceName m_WaypointType;
	
	SCR_EntityWaypoint m_Waypoint = null;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		/*
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		Diag.Escape(Character, Player);
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		SCR_AIFollowBehavior action = new SCR_AIFollowBehavior(utility, null, Player);
		utility.AddAction(action);
		*/
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		Diag.Escape(Character, Player);
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
		group.RemoveAgent(agent);
		Resource groupbase = Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et");
		EntitySpawnParams myparams = EntitySpawnParams();
		myparams.TransformMode = ETransformMode.WORLD;
		myparams.Transform[3] = Character.GetOrigin();
		SCR_AIGroup newgroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupbase, GetGame().GetWorld(), myparams));
		newgroup.AddAgent(agent);
		if (!m_Waypoint)
		{
			Resource wpRes = Resource.Load(m_WaypointType);
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = Player.GetOrigin();
			m_Waypoint = SCR_EntityWaypoint.Cast(GetGame().SpawnEntityPrefab(wpRes, GetGame().GetWorld(), params));
		}
		m_Waypoint.SetEntity(SCR_ChimeraCharacter.Cast(Player));
		Player.AddChild(m_Waypoint, -1);
		newgroup.AddWaypointAt(m_Waypoint, 0);
	}
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	};
}