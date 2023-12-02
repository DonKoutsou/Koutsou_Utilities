[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_KillTask: SP_Task
{
	override int GetTaskType()
	{
		return ETaskType.KILL;
	}

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