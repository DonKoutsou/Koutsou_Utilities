//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_RescueTask: SP_Task
{
	[Attribute(defvalue: "2", desc: "Max amount of rescue tasks that can exist")]
	int m_iMaxamount;
	
	
	ref array <IEntity> CharsToRescue = ;
	int GetMaxamount()
	{
		return m_iMaxamount;
	};
	array <IEntity> GetCharsToResc()
	{
		return CharsToRescue;
	}
	void OnCharacterRescued(EDamageType dType, HitZone hz)
	{
		if (dType != EDamageType.BLEEDING)
			return;
		IEntity Instigator;
		ref array <IEntity> CharsRescued = new ref array <IEntity>();
		foreach (IEntity Rescued : CharsToRescue)
		{
			SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Rescued.FindComponent(SCR_CharacterDamageManagerComponent));
			array<HitZone> blhitZones = new array<HitZone>();
			dmg.GetBleedingHitZones(blhitZones);
			if(blhitZones.IsEmpty())
			{
				dmg.SetResilienceRegenScale(0.3);
				dmg.FullHeal(false);
				dmg.GetOnDamageOverTimeRemoved().Remove(OnCharacterRescued);
				if (dmg.GetInstigator())
					Instigator = dmg.GetInstigator();
				CharsRescued.Insert(Rescued);
				
			}
		}
		for (int i = CharsRescued.Count() - 1; i >= 0; --i)
		{
			CharsToRescue.RemoveItem(CharsRescued[i]);
		}
		if (CharsToRescue.IsEmpty())
		{
			if (Instigator)
				CompleteTask(Instigator);
			else
				CompleteTask(SCR_EntityHelper.GetPlayer());
		}
		else
		{
			if (m_TaskMarker)
				m_TaskMarker.SetOrigin(CharsToRescue.GetRandomElement().GetOrigin());
			else
				AssignCharacter(Instigator);
		}
	}
	override	bool GiveReward(IEntity Target)
	{
		if (m_Reward)
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = Target.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(m_Reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, GetGame().GetWorld(), params));
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
	override bool Init()
	{
		CharsToRescue = new ref array <IEntity>();
		array <ref SP_Task> tasks = new array <ref SP_Task>();
		SP_RequestManagerComponent.GetTasksOfSameType(tasks, SP_RescueTask);
		SP_RescueTask tasksample = SP_RescueTask.Cast(SP_RequestManagerComponent.GetTaskSample(SP_RescueTask));
		if(!tasksample)
		{
			return false;
		}
		m_iMaxamount = tasksample.GetMaxamount();
		if(tasks.Count() >= m_iMaxamount)
		{
			return false;
		}
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
		//FindOwner(m_eTaskOwner);
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		CreateDescritions();
		e_State = ETaskState.UNASSIGNED;
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(FailTask);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(CreateDescritions);
		return true;
	};
	override bool CheckOwner()
	{
		if (!m_eTaskOwner)
			return false;
		if (m_eTaskOwner == SCR_EntityHelper.GetPlayer())
			return false;
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(ScriptedDamageManagerComponent));
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
	override void CreateDescritions()
	{
		string OName;
		string OLoc;
		GetInfo(OName, OLoc);
		m_sTaskDesc = string.Format("%1's squad was ambussed, they need someone to rescuer them, they are somewhere areound %2", OName, OLoc);
		m_sTaskDiag = string.Format("We havent been able to establish connections with %1's squad for a while, please go to %2 and look for them", OName, OLoc);
		m_sTaskTitle = string.Format("Rescue: locate %1's squad and provide help", OName);
		m_sTaskCompletiontext = "We owe you our lives %1, thanks for saving us.";
	};
	override void SpawnTaskMarker(IEntity Assignee)
	{
		Resource Marker = Resource.Load("{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et");
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		m_eTaskOwner.GetWorldTransform(PrefabspawnParams.Transform);
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		m_TaskMarker.SetTitle(m_sTaskTitle);
		m_TaskMarker.SetDescription(m_sTaskDesc);
		m_TaskMarker.SetTarget(m_eTaskOwner);
		m_TaskMarker.SetTargetFaction(Aff.GetAffiliatedFaction());
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned[0]);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	void GetInfo(out string OName, out string OLoc)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		if (m_eTaskOwner)
		{
			OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
			OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		}
	};
	override bool FindOwner(out IEntity Owner)
	{
		if (!CreateVictim(Owner))
		{
			return false;
		}
		if(Owner)
		{
			array<AIAgent> outAgents = new array<AIAgent>();
			AIControlComponent comp = AIControlComponent.Cast(Owner.FindComponent(AIControlComponent));
			AIAgent disagent = comp.GetAIAgent();
			SCR_AIGroup luckygroup = SCR_AIGroup.Cast(disagent.GetParentGroup());
			luckygroup.GetAgents(outAgents);
			return true;
		}
		return false;
	};
	override void FailTask(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		if (!CharsToRescue.IsEmpty())
		{
			foreach (IEntity Char : CharsToRescue)
			{
				SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
				if (!dmgman.IsDestroyed())
				{
					return;
				}
			}
		}
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("Rescue targets have died, task failed"));
		}
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(FailTask);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Remove(CreateDescritions);
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		e_State = ETaskState.FAILED;
		reqman.OnTaskFailed(this);
	}
	override bool CompleteTask(IEntity Assignee)
	{
		if(CharsToRescue.Count() != 0)
		{
			return false;
		}
		if (Assignee && GiveReward(Assignee))
		{
				m_eCopletionist = Assignee;
		}
		if (m_TaskMarker)
		{
			m_TaskMarker.Finish(true);
		}
		e_State = ETaskState.COMPLETED;
		GetOnTaskFinished(this);
		SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
		//SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		//Diag.SendText(GetCompletionText(Assignee), Diag.m_ChatChannelUS, 0, Diag.GetCharacterName(m_eTaskOwner), Diag.GetCharacterRankName(m_eTaskOwner));
		return true;
	};
	override typename GetClassName(){return SP_RescueTask;};
	private bool QueryEntitiesForCharacter(IEntity e)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(e);
		if (!char)
			return true;
		if (!CharsToRescue.Contains(e))
			return false;
		return true;
	}
	private bool CheckForCharacters(float radius, vector origin)
	{
		BaseWorld world = GetGame().GetWorld();
		return GetGame().GetWorld().QueryEntitiesBySphere(origin, radius, QueryEntitiesForCharacter);
	}
	
	bool CreateVictim(out IEntity Victim)
	{
		array<AIAgent> outAgents = new array<AIAgent>();
		IEntity Owner;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
			if (group)
				Owner = group.GetLeaderEntity();
			else
				Owner = GetGame().FindEntity(m_sTaskOwnerOverride);
		}
		else
		{
			if (!m_eTaskOwner)
			{
				//if (!CharacterHolder.GetRandomUnit(ChimeraCharacter.Cast(Owner)))
					//return false;
			}
			else
				Owner = ChimeraCharacter.Cast(m_eTaskOwner);
		}
		
		AIControlComponent ContComp = AIControlComponent.Cast(Owner.FindComponent(AIControlComponent));
		AIAgent Agent = ContComp.GetAIAgent();
		if (!Agent)
			return false;
		SCR_AIGroup luckygroup = SCR_AIGroup.Cast(Agent.GetParentGroup());
		if(!luckygroup)
		{
			return false;
		}
		luckygroup.GetAgents(outAgents);
		if(outAgents.Count() <= 0 || outAgents.Count() >= 5)
		{
			return false;
		}
		Victim = Owner;
		if(!Victim)
		{
			return false;
		}
		foreach(AIAgent agent : outAgents)
		{
			CharsToRescue.Insert(agent.GetControlledEntity());
		}
		//if (!CheckForCharacters(400, Victim.GetOrigin()))
			//return false;
		foreach(AIAgent agent : outAgents)
		{
			IEntity Char = agent.GetControlledEntity();
			if(Char)
			{
				SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
				if(!dmg.GetIsUnconscious())
				{
					dmg.ForceUnconsciousness();
					dmg.SetResilienceRegenScale(0);
					dmg.AddParticularBleeding();
				}
				dmg.SetResilienceRegenScale(0);
				dmg.GetOnDamageOverTimeRemoved().Insert(OnCharacterRescued);
				dmg.GetOnDamageStateChanged().Insert(FailTask);
			}
		}
		return true;
	};
}