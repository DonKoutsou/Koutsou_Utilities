//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps()]
class SP_Task
{
	[Attribute(defvalue : "1", desc : "Dissabled wont be randomly spawned but still exist as samples")]
	bool m_bEnabled;
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
	EEditableEntityLabel RewardLabel;
	//-------------------------------------------------//
	private ref ScriptInvoker s_OnTaskFinished = new ref ScriptInvoker();
	//------------------------------------------------------------------------------------------------------------//
	bool FindOwner(out IEntity Owner){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool FindTarget(out IEntity Target){return true;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetOwner(){return TaskOwner;};
	IEntity GetTarget(){return TaskTarget;};
	//------------------------------------------------------------------------------------------------------------//
	ScriptInvoker OnTaskFinished()
	{
		return s_OnTaskFinished;
	}
	event void GetOnTaskFinished(SP_Task Task)
	{
		OnTaskFinished().Invoke(Task);
	};
	bool CheckOwner()
	{
		if (!TaskOwner)
			return false;
		if (TaskOwner == SCR_EntityHelper.GetPlayer())
			return false;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		if(dmg.GetIsUnconscious())
		{
			return false;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if (!m_RequestManager || !Diag)
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		//Check if char can get more tasks
		m_RequestManager.GetCharTasks(TaskOwner, tasks);
		if(tasks.Count() >= m_RequestManager.GetTasksPerCharacter())
		{
			return false;
		}
		//Check if char can get more tasks of same type
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		m_RequestManager.GetCharTasksOfSameType(TaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= m_RequestManager.GetTasksOfSameTypePerCharacter())
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
	bool CheckTarget()
	{
		if (!TaskTarget)
			return false;
		if (TaskTarget == SCR_EntityHelper.GetPlayer())
			return false;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(TaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		if (!m_RequestManager || !Diag)
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		m_RequestManager.GetCharTargetTasks(TaskTarget, tasks);
		if(tasks.Count() >= m_RequestManager.GetTasksPerCharacter())
		{
			return false;
		}
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		m_RequestManager.GetCharTasksOfSameType(TaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= m_RequestManager.GetTasksOfSameTypePerCharacter() + 1)
		{
			return false;
		}
		return true;
	}
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
			params.Transform[3] = TaskOwner.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, Target.GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				TargetInv.TryInsertItem(Rewardlist[i]);
				Movedamount += 1;
			}
			
			string curr = FilePath.StripPath(reward);
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 %2 added to your inventory, and your reputation has improved", Movedamount.ToString(), curr.Substring(0, curr.Length() - 3)));
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool CompleteTask(IEntity Assignee)
	{
		if (GiveReward(Assignee))
		{
			m_TaskMarker.Finish(true);
			SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
			dmgmn.GetOnDamageStateChanged().Remove(FailTask);
			e_State = ETaskState.COMPLETED;
			m_Copletionist = Assignee;
			GetOnTaskFinished(this);
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: TaskTitle);
			return true;
		}
		return false;
	};
	void FailTask(EDamageState state)
	{
		
		if (state != EDamageState.DESTROYED)
			return;
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("%1 has died, task failed", Diag.GetCharacterName(TaskOwner)));
		}
		e_State = ETaskState.FAILED;
		GetOnTaskFinished(this);
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
	//------------------------------------------------------------------------------------------------------------//
	bool Init()
	{
		m_RequestManager = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		//-------------------------------------------------//
		if (!TaskOwner)
		{
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
		}
		//-------------------------------------------------//
		if (!TaskTarget)
		{
			if (!FindTarget(TaskTarget))
			{
				return false;
			}
			if(!CheckTarget())
			{
				return false;
			}
		}
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