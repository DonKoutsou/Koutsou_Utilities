//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps()]
class SP_Task
{
	//-------------------------------------------------//
	//Character wich created the task
	IEntity TaskOwner;
	//-------------------------------------------------//
	//Target of the task (kill task, deliver task etc... not necesarry on retrieve task)
	IEntity TaskTarget;
	//-------------------------------------------------//
	string TaskDesc;
	//-------------------------------------------------//
	string TaskDiag;
	//-------------------------------------------------//
	string TaskTitle;
	//-------------------------------------------------//
	ResourceName reward;
	//-------------------------------------------------//
	int m_iRewardAmount;
	//-------------------------------------------------//
	protected ETaskState e_State = ETaskState.EMPTY;
	//-------------------------------------------------//
	//Characters assigned to this task
	ref array <IEntity> a_TaskAssigned = new ref array <IEntity>();
	//-------------------------------------------------//
	IEntity m_Copletionist;
	//-------------------------------------------------//
	SP_BaseTask m_TaskMarker;
	//-------------------------------------------------//
	SP_RequestManagerComponent m_RequestManager;
	//------------------------------------------------------------------------------------------------------------//
	bool Init()
	{
		m_RequestManager = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		//-------------------------------------------------//
		//first look for owner cause targer is usually derived from owner faction/location etc...
		if (!FindOwner(TaskOwner))
		{
			return false;
		}
		//-------------------------------------------------//
		//function to fill to check ckaracter
		if(!CheckOwner())
		{
			return false;
		}
		//-------------------------------------------------//
		if (!FindTarget(TaskTarget))
		{
			return false;
		}
		//-------------------------------------------------//
		//function to fill if task needs an entity, eg package for delivery
		//if (!SetupTaskEntity())
		//{
		//	DeleteLeftovers();
		//	return false;
		//}
		//-------------------------------------------------//
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		CreateDescritions();
		e_State = ETaskState.UNASSIGNED;
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(FailTask);
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool FindOwner(out IEntity Owner){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool CheckOwner()
	{
		if (!TaskOwner)
			return false;
		if (TaskOwner == SCR_EntityHelper.GetPlayer())
			return false;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmg)
			return false;
		if(dmg.GetIsUnconscious())
		{
			return false;
		}
		SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if (!ReqMan || !Diag)
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		ReqMan.GetCharTasks(TaskOwner, tasks);
		if(tasks.Count() >= ReqMan.GetTasksPerCharacter())
		{
			return false;
		}
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		ReqMan.GetCharTasksOfSameType(TaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= ReqMan.GetTasksOfSameTypePerCharacter())
		{
			return false;
		}
		string charname = Diag.GetCharacterName(TaskOwner);
		string charrank = Diag.GetCharacterRankName(TaskOwner);
		if(charname == " " || charrank == " ")
		{
			return false;
		}
		Faction senderFaction = Diag.GetCharacterFaction(TaskOwner);
		if (!senderFaction)
			return false;
		if (senderFaction.GetFactionKey() == "RENEGADE")
		{
			return false;
		};
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterIsOwner(IEntity Character)
	{
		if (Character == TaskOwner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetOwner(){return TaskOwner;};
	IEntity GetTarget(){return TaskTarget;};
	//------------------------------------------------------------------------------------------------------------//
	bool FindTarget(out IEntity Target){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterIsTarget(IEntity Character)
	{
		if (Character == TaskTarget)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool SetupTaskEntity(){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool AssignReward()
	{
		EEditableEntityLabel RewardLabel;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			m_iRewardAmount = 10;
		}
		if(index == 1)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		return true;
	};
	void CreateDescritions(){};
	//------------------------------------------------------------------------------------------------------------//
	bool GiveReward(IEntity Target)
	{
		if (reward)
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = vector.Zero;
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, Target.GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				if(TargetInv.TryInsertItem(Rewardlist[i]) == false)
				{
					return false;
				}
				Movedamount += 1;
			}
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 %2 added to your inventory, and your reputation has improved", Movedamount.ToString(), FilePath.StripPath(reward)));
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool CompleteTask(IEntity Assignee)
	{
		
		m_TaskMarker.Finish(true);
		if (GiveReward(Assignee))
		{
			e_State = ETaskState.COMPLETED;
			m_Copletionist = Assignee;
			SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			reqman.OnTaskCompleted(this);
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: TaskDesc);
			return true;
		}
		return false;
	};
	void FailTask(EDamageState state)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		if (state != EDamageState.DESTROYED)
			return;
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("%1 has died, task failed", Diag.GetCharacterName(TaskOwner)));
		}
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		e_State = ETaskState.FAILED;
		reqman.OnTaskFailed(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	string GetTaskDescription(){return TaskDesc;}
	//------------------------------------------------------------------------------------------------------------//
	string GetTaskDiag(){return TaskDiag;}
	//------------------------------------------------------------------------------------------------------------//
	void AssignCharacter(IEntity Character)
	{
		if (a_TaskAssigned.Contains(Character))
			return;
		a_TaskAssigned.Insert(Character);
		if(a_TaskAssigned.Count() > 0 && e_State == ETaskState.UNASSIGNED)
		{
			e_State = ETaskState.ASSIGNED;
		}
		SpawnTaskMarker(Character);
	}
	//------------------------------------------------------------------------------------------------------------//
	void SpawnTaskMarker(IEntity Assignee)
	{
		Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		TaskTarget.GetWorldTransform(PrefabspawnParams.Transform);
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		m_TaskMarker.SetTitle(TaskTitle);
		m_TaskMarker.SetDescription(TaskDesc);
		m_TaskMarker.SetTarget(TaskTarget);
		m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(a_TaskAssigned[0]);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterAssigned(IEntity Character)
	{
		if(a_TaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	typename GetClassName(){return SP_Task;}
	//------------------------------------------------------------------------------------------------------------//
	void DeleteLeftovers(){};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetCompletionist(){return m_Copletionist;};
};
//------------------------------------------------------------------------------------------------------------//
enum ETaskState
{
	UNASSIGNED,
	ASSIGNED,
	COMPLETED,
	FAILED,
	EMPTY
}