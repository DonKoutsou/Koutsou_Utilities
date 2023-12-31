[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_DestroyTask: SP_Task
{

	void UpdateTaskPointer(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(UpdateTaskPointer);
		/*if (m_TaskMarker)
		{
			m_TaskMarker.Finish(true);
			Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
			EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(dmgman.GetInstigator().GetInstigatorEntity().FindComponent(FactionAffiliationComponent));
			m_eTaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
			m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
			m_TaskMarker.SetTitle("Return");
			m_TaskMarker.SetDescription(string.Format("Return to %1 %2 to claim your reward.", DS_DialogueComponent.GetCharacterRankName(m_eTaskOwner), DS_DialogueComponent.GetCharacterName(m_eTaskOwner)));
			m_TaskMarker.SetTarget(m_eTaskOwner);
			m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
			int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned);
			SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
			m_TaskMarker.AddAssignee(assignee, 0);
		}*/
	}
	override bool CheckTarget()
	{
		if (!m_eTaskTarget)
			return false;
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);
		m_sTaskDesc = string.Format("%1 wants someone to destroy a %2, %1 is on %3, go meet him to give you more details if you are interested", OName, DName, OLoc);
		m_sTaskDiag = string.Format("I want you to destroy a %1 located on %2, come back to me after to claim your reward, if the task is too much for you dont risk your life over a few drachmas.", DName, DLoc);
		m_sTaskTitle = string.Format("Destroy: %1", DName);
		m_sTaskCompletiontext = "Good job %1, that will cause some ruckus in their ranks, better keep them on their toes.";
		m_sAcceptTest = string.Format("I'll destroy the %1 for you.", DName);
		m_sacttext = string.Format("The %1 is destroyed." , DName); 
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if(!CharacterAssigned(Assignee))
		{
			return false;
		}
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
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
		OName = SCR_CharacterRankComponent.GetCharacterRankName(m_eTaskOwner) + " " + DS_DialogueComponent.GetCharacterName(m_eTaskOwner);
		DName = DS_DialogueComponent.GetEditableEntName(m_eTaskTarget);
		OLoc = DS_DialogueComponent.GetCharacterLocation(m_eTaskOwner);
		DLoc = DS_DialogueComponent.GetCharacterLocation(m_eTaskTarget);
	};
	override void AddOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(FailTask);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(CreateDescritions);
	}
	override void RemoveOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(CreateDescritions);
	}
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

	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_DestroyTask;};
	//------------------------------------------------------------------------------------------------------------//
};
//------------------------------------------------------------------------------------------------------------//