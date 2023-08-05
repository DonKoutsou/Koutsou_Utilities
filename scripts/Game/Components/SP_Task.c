//------------------------------------------------------------------------------------------------------------//
enum ETaskState
{
	UNASSIGNED,
	ASSIGNED,
	COMPLETED,
	FAILED,
	EMPTY
}
//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps()]
class SP_Task
{
	//-------------------------------------------------//
	[Attribute(defvalue : "1", desc : "Dissabled wont be randomly spawned but still exist as samples")]
	bool m_bEnabled;
	//-------------------------------------------------//
	[Attribute(desc : "Override with entity name in the world wich will become task owner instead of looking for an owner")]
	string m_sTaskOwnerOverride;
	//-------------------------------------------------//
	[Attribute(desc : "Override with entity name in the world wich will become task target instead of looking for an target")]
	string m_sTaskTargetOverride;
	//-------------------------------------------------//
	[Attribute("0", UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(EEditableEntityLabel))]
	EEditableEntityLabel e_RewardLabel;
	//-------------------------------------------------//
	[Attribute(defvalue: "10", desc: "Reward amount if reward end up being currency")]
	int m_iRewardAverageAmount;
	//-------------------------------------------------//
	//Character wich created the task
	IEntity m_eTaskOwner;
	//-------------------------------------------------//
	//Target of the task (kill task, deliver task etc... not necesarry on retrieve task)
	IEntity m_eTaskTarget;
	//-------------------------------------------------//
	string m_sTaskDesc;
	//-------------------------------------------------//
	string m_sTaskDiag;
	//-------------------------------------------------//
	string m_sTaskTitle;
	//-------------------------------------------------//
	string m_sTaskCompletiontext
	//-------------------------------------------------//
	string m_sacttext
	//-------------------------------------------------//
	ResourceName m_Reward;
	//-------------------------------------------------//
	int m_iRewardAmount;
	//-------------------------------------------------//
	protected ETaskState e_State = ETaskState.EMPTY;
	//-------------------------------------------------//
	//Characters assigned to this task
	ref array <IEntity> m_aTaskAssigned = new ref array <IEntity>();
	//-------------------------------------------------//
	IEntity m_eCopletionist;
	//-------------------------------------------------//
	SP_BaseTask m_TaskMarker;
	//-------------------------------------------------//
	private ref ScriptInvoker s_OnTaskFinished = new ref ScriptInvoker();
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetOwner(){return m_eTaskOwner;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetTarget(){return m_eTaskTarget;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetCompletionist(){return m_eCopletionist;};
	//------------------------------------------------------------------------------------------------------------//
	string GetCompletionText(IEntity Completionist){return string.Format(m_sTaskCompletiontext, SP_DialogueComponent.GetCharacterRankName(Completionist) + " " + SP_DialogueComponent.GetCharacterSurname(Completionist));};
	//------------------------------------------------------------------------------------------------------------//
	ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	string GetTaskDescription(){return m_sTaskDesc;}
	//------------------------------------------------------------------------------------------------------------//
	string GetTaskDiag(){return m_sTaskDiag;}
	//------------------------------------------------------------------------------------------------------------//
	string GetActionText(){return m_sacttext;}
	//------------------------------------------------------------------------------------------------------------//
	ScriptInvoker OnTaskFinished(){return s_OnTaskFinished;}
	//------------------------------------------------------------------------------------------------------------//
	event void GetOnTaskFinished(SP_Task Task){OnTaskFinished().Invoke(Task);};
	//------------------------------------------------------------------------------------------------------------//
	void DeleteLeftovers(){};
	//------------------------------------------------------------------------------------------------------------//
	void CreateDescritions(){};
	//------------------------------------------------------------------------------------------------------------//
	bool SetupTaskEntity(){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool FindOwner(out IEntity Owner){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool FindTarget(out IEntity Target){return true;};
	//------------------------------------------------------------------------------------------------------------//
	typename GetClassName(){return SP_Task;}
	//------------------------------------------------------------------------------------------------------------//
	bool CheckOwner()
	{
		if (!m_eTaskOwner)
			return false;
		if (m_eTaskOwner == SCR_EntityHelper.GetPlayer())
			return false;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		//Check if char can get more tasks
		SP_RequestManagerComponent.GetCharTasks(m_eTaskOwner, tasks);
		if(tasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksPerCharacter())
		{
			return false;
		}
		//Check if char can get more tasks of same type
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharTasksOfSameType(m_eTaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksOfSameTypePerCharacter())
		{
			return false;
		}
		Faction senderFaction = SP_DialogueComponent.GetCharacterFaction(m_eTaskOwner);
		if (!senderFaction)
			return false;
		if (senderFaction.GetFactionKey() == "RENEGADE")
		{
			return false;
		};
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool CheckTarget()
	{
		if (!m_eTaskTarget)
			return false;
		if (m_eTaskTarget == SCR_EntityHelper.GetPlayer())
			return false;
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmg)
			return false;
		if (dmg.IsDestroyed())
			return false;
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharTargetTasks(m_eTaskTarget, tasks);
		if(tasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksPerCharacter())
		{
			return false;
		}
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharTasksOfSameType(m_eTaskOwner, sametasks, GetClassName());
		if(sametasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksOfSameTypePerCharacter() + 1)
		{
			return false;
		}
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterIsOwner(IEntity Character)
	{
		if (Character == m_eTaskOwner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterIsTarget(IEntity Character)
	{
		if (Character == m_eTaskTarget)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool AssignReward()
	{
		if (e_RewardLabel)
			return true;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			e_RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			m_iRewardAmount = 10;
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
	bool GiveReward(IEntity Target)
	{
		if (m_Reward)
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = m_eTaskOwner.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(m_Reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, Target.GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				TargetInv.TryInsertItem(Rewardlist[i]);
				Movedamount += 1;
			}
			
			string curr = FilePath.StripPath(m_Reward);
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
			SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
			dmgmn.GetOnDamageStateChanged().Remove(FailTask);
			e_State = ETaskState.COMPLETED;
			m_eCopletionist = Assignee;
			GetOnTaskFinished(this);
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void FailTask(EDamageState state)
	{
		
		if (state != EDamageState.DESTROYED)
			return;
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("%1 has died, task failed", SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
		}
		e_State = ETaskState.FAILED;
		GetOnTaskFinished(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	void AssignCharacter(IEntity Character)
	{
		if (m_aTaskAssigned.Contains(Character))
			return;
		m_aTaskAssigned.Insert(Character);
		if(m_aTaskAssigned.Count() > 0 && e_State == ETaskState.UNASSIGNED)
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
	bool CharacterAssigned(IEntity Character)
	{
		if(m_aTaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool Init()
	{
		//-------------------------------------------------//
		if (!m_eTaskOwner)
		{
			//first look for owner cause targer is usually derived from owner faction/location etc...
			if (!FindOwner(m_eTaskOwner))
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
		if (!m_eTaskTarget)
		{
			if (!FindTarget(m_eTaskTarget))
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
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(FailTask);
		return true;
	};
};