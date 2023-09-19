//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_RescueTask: SP_Task
{
	[Attribute(defvalue: "2", desc: "Max amount of rescue tasks that can exist")]
	int m_iMaxamount;
	
	[Attribute()]
	ResourceName m_BleedTrigger;
	
	
	ref array <IEntity> m_aCharsToRescue = ;
	ref array <IEntity> m_aRescued = ;
	ref array <IEntity> m_aDeceased = ;
	int GetMaxamount()
	{
		return m_iMaxamount;
	};
	array <IEntity> GetCharsToResce()
	{
		return m_aCharsToRescue;
	}
	void OnCharacterRescued(EDamageType dType, HitZone hz)
	{
		if (dType != EDamageType.BLEEDING)
			return;
		IEntity Instigator;
		foreach (IEntity Rescued : m_aCharsToRescue)
		{
			SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Rescued.FindComponent(SCR_CharacterDamageManagerComponent));
			array<HitZone> blhitZones = new array<HitZone>();
			dmg.GetBleedingHitZones(blhitZones);
			if(blhitZones.IsEmpty())
			{
				if (dmg.GetInstigator())
					Instigator = dmg.GetInstigator();
				dmg.SetResilienceRegenScale(0.3);
				dmg.FullHeal(false);
				dmg.GetOnDamageOverTimeRemoved().Remove(OnCharacterRescued);
				dmg.GetOnDamageStateChanged().Remove(OnCharacterKilled);
				if (!m_aRescued.Contains(Rescued))
					m_aRescued.Insert(Rescued);
			}
		}
		for (int i = m_aRescued.Count() - 1; i >= 0; --i)
		{
			m_aCharsToRescue.RemoveItem(m_aRescued[i]);
			if (Instigator && Instigator == SCR_EntityHelper.GetPlayer())
				SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Units rescued : %1\nUnits died : %2\nRemaining : %3", m_aRescued.Count(), m_aDeceased.Count(), m_aCharsToRescue.Count()));
			
		}
		if (m_aCharsToRescue.IsEmpty())
		{
			if (Instigator)
				EvaluateAndFinish(Instigator);
			else
				EvaluateAndFinish(SCR_EntityHelper.GetPlayer());
		}
		else
		{
			if (m_TaskMarker)
				m_TaskMarker.SetOrigin(m_aCharsToRescue.GetRandomElement().GetOrigin());
			else
				AssignCharacter(Instigator);
		}
	}
	void OnCharacterKilled(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		IEntity my_Instigator;
		foreach (IEntity Character : m_aCharsToRescue)
		{
			SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Character.FindComponent(SCR_CharacterDamageManagerComponent));
			if (!dmg.IsDestroyed())
				continue;
			dmg.GetOnDamageOverTimeRemoved().Remove(OnCharacterRescued);
			dmg.GetOnDamageStateChanged().Remove(OnCharacterKilled);
			m_aDeceased.Insert(Character);
		}
		foreach (IEntity Character : m_aDeceased)
		{
			if (m_aCharsToRescue.Contains(Character))
			{
				m_aCharsToRescue.RemoveItem(Character);
				if (GetAssignee() && GetAssignee() == SCR_EntityHelper.GetPlayer())
					SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("Units rescued : %1\nUnits died : %2\nRemaining : %3", m_aRescued.Count(), m_aDeceased.Count(), m_aCharsToRescue.Count()));
			}
		}
		if (m_aCharsToRescue.IsEmpty())
		{
			EvaluateAndFinish(null);
		}
	}
	void EvaluateAndFinish(IEntity Completionist)
	{
		if (!Completionist && GetAssignee())
		{
			Completionist = GetAssignee();
		}
		if (m_aRescued.Count() > 0 && Completionist)
			CompleteTask(Completionist);
		else
			FailTask();
	}
	override bool Init()
	{
		m_aCharsToRescue = {};
		m_aRescued = {};
		m_aDeceased = {};
		array <ref SP_Task> tasks = new array <ref SP_Task>();
		SP_RequestManagerComponent.GetTasksOfSameType(tasks, SP_RescueTask);
		
		if (!m_bPartOfChain)
		{
			SP_RescueTask tasksample = SP_RescueTask.Cast(SP_RequestManagerComponent.GetTaskSample(SP_RescueTask));
			if(!tasksample)
			{
				return false;
			}
			m_iMaxamount = tasksample.GetMaxamount();
			m_BleedTrigger = tasksample.m_BleedTrigger;
			if(tasks.Count() >= m_iMaxamount)
			{
				return false;
			}
			InheritFromSample();
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
		SetTimeLimit();
		SpawnBleedTrigger();
		CreateDescritions();
		AddOwnerInvokers();
		e_State = ETaskState.UNASSIGNED;
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
		m_OwnerFaction = SCR_Faction.Cast(senderFaction);
		if (m_OwnerFaction.GetFactionKey() == "RENEGADE")
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
		m_sTaskDesc = string.Format("%1's squad was ambussed, someone ought to go look for them, they are somewhere areound %2.", OName, OLoc);
		m_sTaskDiag = string.Format("We've lost coms with %1's squad for a while, please go to %2 and look for them", OName, OLoc);
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
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned);
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
	override void FailTask()
	{
		if (!m_aCharsToRescue.IsEmpty())
		{
			foreach (IEntity Char : m_aCharsToRescue)
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
		if(m_aCharsToRescue.Count() != 0)
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
		if (SCR_EntityHelper.GetPlayer() == Assignee)
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
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(e.FindComponent(ScriptedDamageManagerComponent));
		if (dmg.GetState() == EDamageState.DESTROYED)
			return true;
		if (m_aCharsToRescue.Contains(e))
			return true;
		return false;
	}
	private bool CheckForCharacters(float radius, vector origin)
	{
		BaseWorld world = GetGame().GetWorld();
		FactionAffiliationComponent Myaffiliation = FactionAffiliationComponent.Cast(m_aCharsToRescue[0].FindComponent(FactionAffiliationComponent));
		array<IEntity> entities = {};
		GetGame().GetTagManager().GetTagsInRange(entities, origin, radius, ETagCategory.NameTag);
		foreach (IEntity Char : entities)
		{
			if (m_aCharsToRescue.Contains(Char))
				continue;
			FactionAffiliationComponent affiliation = FactionAffiliationComponent.Cast(Char.FindComponent(FactionAffiliationComponent));
			if (affiliation.GetAffiliatedFaction().IsFactionFriendly(Myaffiliation.GetAffiliatedFaction()))
			{
				return true;
			}
		}
		return false;
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
				ChimeraCharacter char;
				if (!CharacterHolder.GetRandomUnit(char))
					return false;
				Owner = char;
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
			m_aCharsToRescue.Insert(agent.GetControlledEntity());
		}
		if (!m_bPartOfChain && !CheckForCharacters(400, Owner.GetOrigin()))
			return false;
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
				}
				dmg.SetResilienceRegenScale(0);
				dmg.GetOnDamageOverTimeRemoved().Insert(OnCharacterRescued);
				dmg.GetOnDamageStateChanged().Insert(OnCharacterKilled);
			}
		}
		return true;
	};
	override void GetOnOwnerDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveOwnerInvokers();
		if (!m_aCharsToRescue.IsEmpty())
		{
			m_eTaskOwner = m_aCharsToRescue[0];
			AddOwnerInvokers();
		}
	}
	override void AddTargetInvokers()
	{
	};
	override void RemoveTargetInvokers()
	{
	};
	void SpawnBleedTrigger()
	{
		if (!m_BleedTrigger)
			m_BleedTrigger = "{BB079FFC94347494}prefabs/Triggers/SP_BleedTrigger.et";

		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = m_eTaskOwner.GetOrigin();
		SP_CharacterTriggerEntity trigger = SP_CharacterTriggerEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_BleedTrigger), GetGame().GetWorld(), params));
		trigger.AddCharacters(m_aCharsToRescue);
		//m_eTaskOwner.AddChild(trigger, 1);
	}
}