[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_KillTask: SP_Task
{
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
		}
		else
		{
			if(!CharacterHolder.GetRandomUnit(Char))
				return false;
		}
		
		if (Char)
			Owner = Char;
		if(Owner)
		{
			return true;
		}
		return false;
	};
	void UpdateTaskPointer(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(UpdateTaskPointer);
		DS_DialogueComponent diag = DS_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(DS_DialogueComponent));
		if (m_TaskMarker)
		{
			m_TaskMarker.Finish(true);
			Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
			EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(dmgman.GetInstigator().GetInstigatorEntity().FindComponent(FactionAffiliationComponent));
			m_eTaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
			m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
			m_TaskMarker.SetTitle("Return");
			m_TaskMarker.SetDescription(string.Format("Return to %1 %2 to claim your reward.", diag.GetCharacterRankName(m_eTaskOwner), diag.GetCharacterName(m_eTaskOwner)));
			m_TaskMarker.SetTarget(m_eTaskOwner);
			m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
			int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned);
			SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
			m_TaskMarker.AddAssignee(assignee, 0);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool FindTarget(out IEntity Target)
	{
		ChimeraCharacter Char;
		if (m_sTaskTargetOverride && GetGame().FindEntity(m_sTaskTargetOverride))
		{
			SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().FindEntity(m_sTaskTargetOverride));
			if (group)
				Char = ChimeraCharacter.Cast(group.GetLeaderEntity());
			else
				Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskTargetOverride));
		}
		else
		{
			FactionAffiliationComponent AffiliationComp = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
			SCR_FactionManager FactionMan = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			array <Faction> enemies = new array <Faction>();
			FactionMan.GetEnemyFactions(Fact, enemies);
			if (enemies.IsEmpty())
				return false;
			
			if (!CharacterHolder.GetFarUnitOfFaction(ChimeraCharacter.Cast(GetOwner()), 300, enemies.GetRandomElement(), Char))
				return false;
		}
		
		if (Char)
			Target = Char;
		
		if (Target == GetOwner())
			return false;
		if(Target)
			return true;
		
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);
		m_sTaskDesc = string.Format("%1 is looking for someone to locate and eliminate %2. %1 is on %3, go meet him to give you more details if you are interested", OName, DName, OLoc);
		m_sTaskDiag = string.Format("I want you to kill %1, he should be on %2.", DName, DLoc);
		m_sTaskTitle = string.Format("Kill %1", DName);
		m_sAcceptTest = string.Format("I'll go for %1's head.", DName);
		m_sTaskCompletiontext = "Good job %1, he got what he deserved, thanks, hope the reward will suffice.";
		m_sacttext = string.Format("%1 is dead.", DName);
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if(!CharacterAssigned(Assignee))
		{
			return false;
		}
		SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (dmgman.IsDestroyed())
			return true;
		return false;			
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!m_eTaskOwner || !m_eTaskTarget)
		{
			return;
		}
		DS_DialogueComponent Diag = DS_DialogueComponent.Cast(SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = CharRank.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	override void AddTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(UpdateTaskPointer);
	}
	override void RemoveTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(UpdateTaskPointer);
	}
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_KillTask;};
	//------------------------------------------------------------------------------------------------------------//
};
//------------------------------------------------------------------------------------------------------------//