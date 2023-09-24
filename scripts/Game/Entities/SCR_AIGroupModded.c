modded class SCR_AIGroup
{
	bool inited;
	//------------------------------------------------------------------------------------------------
	bool moddedAddAIEntityToGroup(IEntity entity, bool islast = 0)
	{
		if (!entity) return false;
		
		AIControlComponent control = AIControlComponent.Cast(entity.FindComponent(AIControlComponent));
		if (!control) return false;
		
		AIAgent agent = control.GetControlAIAgent();
		if (!agent) return false;
		
		control.ActivateAI();
		
		if (!agent.GetParentGroup())
			AddAgent(agent); //--- Add to group only if some other system (e.g., component on the group member) wasn't faster
		
		if (islast)
		{
			inited = true;
		}
		
		return true;
	}
	override protected void SpawnGroupMember(bool snapToTerrain, int index, ResourceName res, bool editMode, bool isLast)
	{
		if (!GetGame().GetAIWorld().CanAICharacterBeAdded())
		{
			if (isLast)
			{
				Event_OnInit.Invoke(this);
				inited = true;
			}
				
			
			//Event_OnLastGroupMemberSpawned.Invoke(this);
			
			return;
		}
		BaseWorld world = GetWorld();
		AIFormationDefinition formationDefinition;
		AIFormationComponent formationComponent = AIFormationComponent.Cast(this.FindComponent(AIFormationComponent));
		if (formationComponent)
			formationDefinition = formationComponent.GetFormation();
		EntitySpawnParams spawnParams = new EntitySpawnParams;
		spawnParams.TransformMode = ETransformMode.WORLD;
		GetWorldTransform(spawnParams.Transform);
		vector pos = spawnParams.Transform[3];
		
		if (formationDefinition)
			pos = CoordToParent(formationDefinition.GetOffsetPosition(index));
		else
			pos = CoordToParent(Vector(index, 0, 0));
		
		
		
		float surfaceY = world.GetSurfaceY(pos[0], pos[2]);
		if (snapToTerrain && pos[1] < surfaceY)
		{
			pos[1] = surfaceY;
		}
		
		//Snap to the nearest navmesh point
		AIPathfindingComponent pathFindindingComponent = AIPathfindingComponent.Cast(this.FindComponent(AIPathfindingComponent));
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(pos, "10 10 10", pos))
		{
			float groundHeight = world.GetSurfaceY(pos[0], pos[2]);
			if (pos[1] < groundHeight)
				pos[1] = groundHeight;
		}
		
		spawnParams.Transform[3] = pos;
		
		IEntity member = GetGame().SpawnEntityPrefab(Resource.Load(res), world, spawnParams);
		if (!member)
			return;
		
		if (editMode)
			m_aSceneGroupUnitInstances.Insert(member);
		
		moddedAddAIEntityToGroup(member, isLast);
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(member.FindComponent(FactionAffiliationComponent));
		
		if (factionAffiliation)
			factionAffiliation.SetAffiliatedFactionByKey(m_faction);
	
		if (isLast)
		{
			Event_OnInit.Invoke(this);
			inited = true;
		}
	}
	
	override void OnLeaderChanged(AIAgent currentLeader, AIAgent prevLeader)
	{
		if (!currentLeader)
			return;
		Event_OnLeaderChanged.Invoke(currentLeader, prevLeader);
		
		if (currentLeader && currentLeader.GetControlledEntity())
		{
			EventHandlerManagerComponent eventHandlerManagerComponent = EventHandlerManagerComponent.Cast(currentLeader.GetControlledEntity().FindComponent(EventHandlerManagerComponent));
			if (eventHandlerManagerComponent)
				eventHandlerManagerComponent.RegisterScriptHandler("OnConsciousnessChanged", this, this.OnLeaderConsciousnessChanged, true);
		}
		
		if (prevLeader && prevLeader.GetControlledEntity())
		{
			EventHandlerManagerComponent eventHandlerManagerComponent = EventHandlerManagerComponent.Cast(prevLeader.GetControlledEntity().FindComponent(EventHandlerManagerComponent));
			if (eventHandlerManagerComponent)
				eventHandlerManagerComponent.RemoveScriptHandler("OnConsciousnessChanged", this, this.OnLeaderConsciousnessChanged, true);
		}
		SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(currentLeader.GetControlledEntity().FindComponent(SCR_CharacterRankComponent));
		SCR_ECharacterRank m_CurrentRank = Rankcomp.GetCharacterRank(currentLeader.GetControlledEntity());
		switch (m_CurrentRank)
		{
		case SCR_ECharacterRank.PRIVATE:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.CORPORAL);
			}
			break;
		case SCR_ECharacterRank.CORPORAL:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.SERGEANT);
			}
			break;
		case SCR_ECharacterRank.SERGEANT:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.LIEUTENANT);
			}
			break;
		case SCR_ECharacterRank.LIEUTENANT:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.CAPTAIN);
			}
			break;
		case SCR_ECharacterRank.CAPTAIN:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.MAJOR);
			}
			break;
		case SCR_ECharacterRank.MAJOR:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.COLONEL);
			}
			break;
		case SCR_ECharacterRank.COLONEL:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.COMMANDER);
			}
			break;
		}
	}
};