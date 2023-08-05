[BaseContainerProps(configRoot:true)]
class SP_DestroyTask: SP_Task
{
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
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
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(UpdateTaskPointer);
		if (m_TaskMarker)
		{
			m_TaskMarker.Finish(true);
			Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
			EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(dmgman.GetInstigator().FindComponent(FactionAffiliationComponent));
			m_eTaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
			m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
			m_TaskMarker.SetTitle("Return");
			m_TaskMarker.SetDescription(string.Format("Return to %1 %2 to claim your reward.", SP_DialogueComponent.GetCharacterRankName(m_eTaskOwner), SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
			m_TaskMarker.SetTarget(m_eTaskOwner);
			m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
			int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned[0]);
			SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
			m_TaskMarker.AddAssignee(assignee, 0);
		}
	}
	override void SpawnTaskMarker(IEntity Assignee)
	{
		Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		m_eTaskTarget.GetWorldTransform(PrefabspawnParams.Transform);
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		m_TaskMarker.SetTitle(m_sTaskTitle);
		m_TaskMarker.SetDescription(m_sTaskDesc);
		m_TaskMarker.SetTarget(m_eTaskTarget);
		m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned[0]);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool FindTarget(out IEntity Target)
	{
		if (m_sTaskTargetOverride && GetGame().FindEntity(m_sTaskTargetOverride))
		{
			Target = GetGame().FindEntity(m_sTaskTargetOverride);
		}
		if(Target)
		{
			ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(Target.FindComponent(ScriptedDamageManagerComponent));
			dmgman.GetOnDamageStateChanged().Insert(UpdateTaskPointer);
			return true;
		}
		return false;
	};
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
		m_sTaskDesc = string.Format("%1 wants someone to destroy a %2 located on %3.", OName, DName, DLoc);
		m_sTaskDiag = string.Format("I want you to destroy a %1 located on %2, come back to me after to claim your reward, if the task is too much for you dont risk your life over a few drachmas.", DName, DLoc);
		m_sTaskTitle = string.Format("Destroy: %1", DName);
		m_sTaskCompletiontext = "Good job %1, that will cause some ruckus in their ranks, better keep them on their toes.";
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
	override bool AssignReward()
	{
		if (e_RewardLabel)
			return true;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(!ReqMan)
			{
				return false;
			}
			SP_DestroyTask tasksample = SP_DestroyTask.Cast(ReqMan.GetTaskSample(SP_DestroyTask));
			if(!tasksample)
			{
				return false;
			}
			m_iRewardAverageAmount = tasksample.GetRewardAverage();
			if(m_iRewardAverageAmount)
			{
				m_iRewardAmount = Math.Floor(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
			}
			else
			{
				m_iRewardAmount = Math.RandomInt(5, 15)
			}
		}
		if(index == 1)
		{
			e_RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(e_RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		m_Reward = entry.GetPrefab();
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool CompleteTask(IEntity Assignee)
	{
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Ownerinv = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
		if (GiveReward(Assignee))
		{
			if (m_TaskMarker)
			{
				m_TaskMarker.Finish(true);
			}
			e_State = ETaskState.COMPLETED;
			m_eCopletionist = Assignee;
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			ScriptedDamageManagerComponent dmgmn = ScriptedDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(ScriptedDamageManagerComponent));
			dmgmn.GetOnDamageStateChanged().Remove(FailTask);
			GetOnTaskFinished(this);
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!m_eTaskOwner || !m_eTaskTarget)
		{
			return;
		}
		OName = SCR_CharacterRankComponent.GetCharacterRankName(m_eTaskOwner) + " " + SP_DialogueComponent.GetCharacterName(m_eTaskOwner);
		DName = SP_DialogueComponent.GetEditableEntName(m_eTaskTarget);
		OLoc = SP_DialogueComponent.GetCharacterLocation(m_eTaskOwner);
		DLoc = SP_DialogueComponent.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//

	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_DestroyTask;};
	//------------------------------------------------------------------------------------------------------------//
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
	};
};
//------------------------------------------------------------------------------------------------------------//