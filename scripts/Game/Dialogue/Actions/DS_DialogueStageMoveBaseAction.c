[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class DS_DialogueStageMoveBaseInhabitantsAction : DS_BaseDialogueStageAction
{
	[Attribute("0",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	SP_BaseEn m_TaskBaseNameFrom;
	
	[Attribute("0",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	SP_BaseEn m_TaskBaseNameTo;
	
	[Attribute()]
	bool IncludePlayer;

	IEntity m_Player;
	ref array <IEntity> m_aInhabitants = {};
	
	override void Perform(IEntity Character, IEntity Player)
	{
		m_Player = GetGame().GetPlayerController().GetControlledEntity();
		
		SCR_GameModeCampaign gm = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		SCR_CampaignMilitaryBaseManager man = gm.GetBaseManager();
		
		string namefrom = SP_BaseNames.Get(m_TaskBaseNameFrom);
		string nameto = SP_BaseNames.Get(m_TaskBaseNameTo);
		
		SCR_CampaignMilitaryBaseComponent basetomovefrom = man.GetNamedBase(namefrom);
		SCR_CampaignMilitaryBaseComponent basetomoveto = man.GetNamedBase(nameto);
		
		float dist = vector.Distance(basetomovefrom.GetOwner().GetOrigin(), basetomoveto.GetOwner().GetOrigin());
		TimeAndWeatherManagerEntity tnwman = ChimeraWorld.CastFrom(GetGame().GetWorld()).GetTimeAndWeatherManager();
		TaskDayTimeInfo timeinfo = TaskDayTimeInfo.FromPointInFuture(dist/1000);
		tnwman.SetTime(timeinfo.GetTime());
		
		GetGame().GetWorld().QueryEntitiesBySphere(basetomovefrom.GetOwner().GetOrigin(), basetomovefrom.GetRadius(), QueryEntitiesForCharacter);
		
		IEntity commander;
		basetomovefrom.GetCharacterOfPost(SCR_ECharacterRank.COMMANDER, commander);
		if (commander)
		{
			ChimeraCharacter char = ChimeraCharacter.Cast(commander);
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(commander.FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(char.GetCharacterController().GetAnimationComponent().GetCommandHandler());
				if (handler.IsLoitering())
				{
					handler.StopLoitering(true);
				}
				vector mat[4];
				mat[3] = basetomoveto.GetOwner().GetOrigin();
				editable.SetTransform(mat);
				if (m_aInhabitants.Contains(commander))
					m_aInhabitants.RemoveItem(commander);
				basetomoveto.RegisterCharacter(commander, SCR_ECharacterRank.COMMANDER);
			}
			AIGroup group = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup();
			AIWaypoint wp = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetCurrentWaypoint();
			AIWaypoint newwp =  CreateWaypointBaseon(wp, basetomoveto.GetOwner().GetOrigin());
			group.RemoveWaypoint(wp);
			group.AddWaypoint(newwp);
		}
		IEntity medic;
		basetomovefrom.GetCharacterOfPost(SCR_ECharacterRank.MEDIC, medic);
		if (medic)
		{
			ChimeraCharacter char = ChimeraCharacter.Cast(medic);
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(medic.FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(char.GetCharacterController().GetAnimationComponent().GetCommandHandler());
				if (handler.IsLoitering())
				{
					handler.StopLoitering(true);
				}
				vector mat[4];
				mat[3] = basetomoveto.GetServiceByType(SCR_EServicePointType.FIELD_HOSPITAL).GetOwner().GetOrigin();
				editable.SetTransform(mat);
				if (m_aInhabitants.Contains(medic))
					m_aInhabitants.RemoveItem(medic);
				basetomoveto.RegisterCharacter(commander, SCR_ECharacterRank.MEDIC);
			}

			AIGroup group = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup();
			AIWaypoint wp = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetCurrentWaypoint();
			AIWaypoint newwp =  CreateWaypointBaseon(wp, basetomoveto.GetServiceByType(SCR_EServicePointType.FIELD_HOSPITAL).GetOwner().GetOrigin());
			group.RemoveWaypoint(wp);
			group.AddWaypoint(newwp);
		}
		IEntity foodvendor;
		basetomovefrom.GetCharacterOfPost(SCR_ECharacterRank.FOOD_VENDOR, foodvendor);
		if (foodvendor)
		{
			ChimeraCharacter char = ChimeraCharacter.Cast(foodvendor);
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(foodvendor.FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(char.GetCharacterController().GetAnimationComponent().GetCommandHandler());
				if (handler.IsLoitering())
				{
					handler.StopLoitering(true);
				}
				vector mat[4];
				mat[3] = basetomoveto.GetServiceByType(SCR_EServicePointType.FIELD_HOSPITAL).GetOwner().GetOrigin();
				editable.SetTransform(mat);
				if (m_aInhabitants.Contains(foodvendor))
					m_aInhabitants.RemoveItem(foodvendor);
				basetomoveto.RegisterCharacter(commander, SCR_ECharacterRank.FOOD_VENDOR);
			}

			AIGroup group = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup();
			AIWaypoint wp = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetCurrentWaypoint();
			AIWaypoint newwp =  CreateWaypointBaseon(wp, basetomoveto.GetServiceByType(SCR_EServicePointType.FIELD_HOSPITAL).GetOwner().GetOrigin());
			group.RemoveWaypoint(wp);
			group.AddWaypoint(newwp);
		}
		IEntity mechanic;
		basetomovefrom.GetCharacterOfPost(SCR_ECharacterRank.MECHANIC, mechanic);
		if (mechanic)
		{
			ChimeraCharacter char = ChimeraCharacter.Cast(mechanic);
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(mechanic.FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(char.GetCharacterController().GetAnimationComponent().GetCommandHandler());
				if (handler.IsLoitering())
				{
					handler.StopLoitering(true);
				}
				vector mat[4];
				mat[3] = basetomoveto.GetServiceByType(SCR_EServicePointType.REPAIR_DEPOT).GetOwner().GetOrigin();
				editable.SetTransform(mat);
				
				if (m_aInhabitants.Contains(mechanic))
					m_aInhabitants.RemoveItem(mechanic);
				basetomoveto.RegisterCharacter(commander, SCR_ECharacterRank.MECHANIC);
			}

			AIGroup group = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup();
			AIWaypoint wp = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetCurrentWaypoint();
			AIWaypoint newwp =  CreateWaypointBaseon(wp, basetomoveto.GetServiceByType(SCR_EServicePointType.REPAIR_DEPOT).GetOwner().GetOrigin());
			group.RemoveWaypoint(wp);
			group.AddWaypoint(newwp);
		}
		if (m_aInhabitants.IsEmpty())
			return;
		
		vector basetomoveor[4];
		basetomoveor[3] = basetomoveto.GetOwner().GetOrigin();
		for (int i; i < m_aInhabitants.Count(); i++)
		{
			ChimeraCharacter char = ChimeraCharacter.Cast(m_aInhabitants[i]);
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(m_aInhabitants[i].FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(char.GetCharacterController().GetAnimationComponent().GetCommandHandler());
				if (handler.IsLoitering())
				{
					handler.StopLoitering(true);
				}
				vector mat[4];
				mat[3] = s_AIRandomGenerator.GenerateRandomPointInRadius(10, 20, basetomoveto.GetOwner().GetOrigin(), true);
				editable.SetTransform(mat);
			}

			AIGroup group = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup();
			AIWaypoint wp = char.GetCharacterController().GetAIControlComponent().GetAIAgent().GetParentGroup().GetCurrentWaypoint();
			AIWaypoint newwp =  CreateWaypointBaseon(wp, basetomoveto.GetOwner().GetOrigin());
			group.RemoveWaypoint(wp);
			group.AddWaypoint(newwp);
		}
		if (IncludePlayer)
		{
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(m_Player.FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				vector mat[4];
				mat[3] = s_AIRandomGenerator.GenerateRandomPointInRadius(10, 20, basetomoveto.GetOwner().GetOrigin(), true);
				editable.SetTransform(mat);
			}
		}
		super.Perform(Character, Player);
	};
	protected AIWaypoint CreateWaypointBaseon(AIWaypoint wp, vector loc)
	{
		if (!wp)
			return null;

		EntitySpawnParams paramsPatrolWP = new EntitySpawnParams();
		paramsPatrolWP.TransformMode = ETransformMode.WORLD;
		paramsPatrolWP.Transform[3] = loc;
		
		Resource resWP = Resource.Load(wp.GetPrefabData().GetPrefabName());
		if (!resWP)
			return null;

		AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resWP, GetGame().GetWorld(), paramsPatrolWP));
		if (!waypoint)
			return null;
		
		return waypoint;
	}
	private bool QueryEntitiesForCharacter(IEntity e)
	{
		if (e == m_Player)
			return true;
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(e);
		if (!char)
			return true;
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(e.FindComponent(ScriptedDamageManagerComponent));
		if (dmg.GetState() == EDamageState.DESTROYED)
			return true;
		
		m_aInhabitants.Insert(e);
		return true;
	}
};
[BaseContainerProps(configRoot:true)]
class DS_3RDNamedAttributeText : DS_BaseDialogueText
{
	[Attribute("0",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	SP_BaseEn m_TaskBaseNameFrom;
	
	[Attribute("20",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SCR_ECharacterRank) ,params:  "20 23 1")]
	SCR_ECharacterRank m_CharPost;
	
	[Attribute(desc: "Inclue the %1 sign on the place where character name will be placed")]
	string m_sActionTextOverride;
	
	[Attribute(defvalue:"0")]
	bool m_bAddRankName;
	
	[Attribute(defvalue:"1")]
	bool m_bAddFirstName;
	
	[Attribute(defvalue:"1")]
	bool m_bAddLastName;
	
	
	override string GetText(IEntity Character, IEntity Player)
	{
		SCR_CampaignMilitaryBaseManager baseman = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
		SCR_CampaignMilitaryBaseComponent base = baseman.GetNamedBase(SP_BaseNames.Get(m_TaskBaseNameFrom));
		IEntity char;
		base.GetCharacterOfPost(m_CharPost, char);
		
		string name;
		
		if (m_bAddRankName)
		{
			string rankname = DS_DialogueComponent.GetCharacterRankName(char);
			name = rankname + " ";
		}
		if (m_bAddFirstName)
		{
			string firstkname = DS_DialogueComponent.GetCharacterName(char);
			name = name + firstkname + " ";
		}
		if (m_bAddLastName)
		{
			string lastkname = DS_DialogueComponent.GetCharacterSurname(char);
			name = name + lastkname;
		}
		return string.Format(m_sActionTextOverride, name);;
	};
}
