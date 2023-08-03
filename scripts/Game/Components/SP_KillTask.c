[BaseContainerProps(configRoot:true)]
class SP_KillTask: SP_Task
{
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		CharacterHolder CharHolder = m_RequestManager.GetCharacterHolder();
		ChimeraCharacter Char;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
		}
		else
		{
			if (CharHolder)
			if(!CharHolder.GetRandomUnit(Char))
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
		SP_DialogueComponent diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		if (m_TaskMarker)
		{
			m_TaskMarker.Finish(true);
			Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
			EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
			FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(dmgman.GetInstigator().FindComponent(FactionAffiliationComponent));
			m_eTaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
			m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
			m_TaskMarker.SetTitle("Return");
			m_TaskMarker.SetDescription(string.Format("Return to %1 %2 to claim your reward.", diag.GetCharacterRankName(m_eTaskOwner), diag.GetCharacterName(m_eTaskOwner)));
			m_TaskMarker.SetTarget(m_eTaskOwner);
			m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
			int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned[0]);
			SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
			m_TaskMarker.AddAssignee(assignee, 0);
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	override bool FindTarget(out IEntity Target)
	{
		CharacterHolder CharHolder = m_RequestManager.GetCharacterHolder();
		ChimeraCharacter Char;
		if (m_sTaskTargetOverride && GetGame().FindEntity(m_sTaskTargetOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskTargetOverride));
		}
		else
		{
			FactionAffiliationComponent AffiliationComp = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
			SP_FactionManager FactionMan = SP_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			array <Faction> enemies = new array <Faction>();
			FactionMan.GetEnemyFactions(Fact, enemies);
			if (enemies.IsEmpty())
				return false;
			
			if (!CharHolder.GetFarUnitOfFaction(ChimeraCharacter.Cast(GetOwner()), 300, enemies.GetRandomElement(), Char))
				return false;
		}
		
		if (Char)
			Target = Char;
		
		if (Target == GetOwner())
			return false;
		SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(Target.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(UpdateTaskPointer);
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
		m_sTaskDesc = string.Format("%1 has put a bounty on %2's head.", OName, DName);
		m_sTaskDiag = string.Format("I want you to kill %1, he should be on %2.", DName, DLoc);
		m_sTaskTitle = string.Format("Kill: assasinate %1", DName);
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
			SP_KillTask tasksample = SP_KillTask.Cast(ReqMan.GetTaskSample(SP_KillTask));
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
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if (GiveReward(Assignee))
		{
			if (m_TaskMarker)
			{
				m_TaskMarker.Finish(true);
			}
			e_State = ETaskState.COMPLETED;
			m_eCopletionist = Assignee;
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
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
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = CharRank.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//

	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_KillTask;};
	//------------------------------------------------------------------------------------------------------------//
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
	};
	override string GetCompletionText(IEntity Completionist)
	{
		SP_DialogueComponent diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		string TaskCompletiontext = string.Format("Good job, he got what he deserved, thanks, hope the reward will suffice.", diag.GetCharacterRankName(Completionist), diag.GetCharacterSurname(Completionist));
		return TaskCompletiontext;
	};
};
//------------------------------------------------------------------------------------------------------------//