//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_RescueTask: SP_Task
{
	[Attribute(defvalue: "20")]
	int m_iRewardAverageAmount;
	
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
		for (int i = CharsToRescue.Count() - 1; i >= 0; --i)
		{
			SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(CharsToRescue[i].FindComponent(SCR_CharacterDamageManagerComponent));
			array<HitZone> blhitZones = new array<HitZone>();
			dmg.GetBleedingHitZones(blhitZones);
			if(blhitZones.IsEmpty())
			{
				dmg.SetResilienceRegenScale(0.3);
				dmg.GetOnDamageOverTimeRemoved().Remove(OnCharacterRescued);
				Instigator = dmg.GetInstigator();
				CharsToRescue.RemoveItem(CharsToRescue[i]);
			}
		}
		if (CharsToRescue.IsEmpty())
		{
			if (Instigator)
				CompleteTask(Instigator);
			else
				CompleteTask(null);
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
		if (reward)
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = Target.GetOrigin();
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, GetGame().GetWorld(), params));
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
	override bool Init()
	{
		CharsToRescue = new ref array <IEntity>();
		m_RequestManager = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		array <ref SP_Task> tasks = new array <ref SP_Task>();
		m_RequestManager.GetTasksOfSameType(tasks, SP_RescueTask);
		SP_RescueTask tasksample = SP_RescueTask.Cast(m_RequestManager.GetTaskSample(SP_RescueTask));
		if(!tasksample)
		{
			return false;
		}
		m_iMaxamount = tasksample.GetMaxamount();
		if(tasks.Count() >= m_iMaxamount)
		{
			return false;
		}
		if (!FindTarget(TaskTarget))
		{
			return false;
		}
		//FindOwner(TaskOwner);
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		CreateDescritions();
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, DLoc, OLoc);
		TaskDesc = string.Format("%1's squad was ambussed, they need someone to rescuer them, they are somewhere areound %2", DName, DLoc);
		TaskDiag = string.Format("We havent been able to establish connections with %1's squad for a while, please go to %2 and look for them", DName, DLoc);
		TaskTitle = string.Format("Rescue: locate %1's squad and provide help", DName);
	};
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskTarget.FindComponent(SCR_CharacterRankComponent));
		if (TaskOwner)
		{
			OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
			OLoc = Diag.GetCharacterLocation(TaskOwner);
		}
		if (TaskTarget)
		{
			DName = CharRank.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget);
			DLoc = Diag.GetCharacterLocation(TaskTarget);
		}
	};
	override bool FindTarget(out IEntity Target)
	{
		if (!CreateVictim(Target))
		{
			return false;
		}
		if(Target)
		{
			array<AIAgent> outAgents = new array<AIAgent>();
			AIControlComponent comp = AIControlComponent.Cast(Target.FindComponent(AIControlComponent));
			AIAgent disagent = comp.GetAIAgent();
			SCR_AIGroup luckygroup = SCR_AIGroup.Cast(disagent.GetParentGroup());
			luckygroup.GetAgents(outAgents);
			Target = luckygroup.GetLeaderEntity();
			return true;
		}
		return false;
	};
	override bool AssignReward()
	{
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
		SP_RescueTask tasksample = SP_RescueTask.Cast(reqman.GetTaskSample(SP_RescueTask));
		m_iRewardAverageAmount = tasksample.GetRewardAverage();
		if(m_iRewardAverageAmount)
			{
				m_iRewardAmount = Math.Floor(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
			}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		return true;
	};
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
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
				m_Copletionist = Assignee;
		}
		if (m_TaskMarker)
		{
				m_TaskMarker.Finish(true);
		}
		e_State = ETaskState.COMPLETED;
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		reqman.OnTaskCompleted(this);
		SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: TaskTitle);
		return false;
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
		CharacterHolder Chars = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent)).GetCharacterHolder();
		array<AIAgent> outAgents = new array<AIAgent>();
		ChimeraCharacter luckyguy;
		if (!Chars.GetRandomUnit(luckyguy))
			return false;
		AIControlComponent ContComp = AIControlComponent.Cast(luckyguy.FindComponent(AIControlComponent));
		AIAgent Agent = ContComp.GetAIAgent();
		if (!Agent)
			return false;
		SCR_AIGroup luckygroup = SCR_AIGroup.Cast(Agent.GetParentGroup());
		if(!luckygroup)
		{
			return false;
		}
		luckygroup.GetAgents(outAgents);
		if(outAgents.Count() <= 0)
		{
			return false;
		}
		
		Victim = luckygroup.GetLeaderEntity();
		if(!Victim)
		{
			return false;
		}
		foreach(AIAgent agent : outAgents)
		{
			CharsToRescue.Insert(agent.GetControlledEntity());
		}
		if (!CheckForCharacters(400, Victim.GetOrigin()))
			return false;
		foreach(AIAgent agent : outAgents)
		{
			IEntity Char = agent.GetControlledEntity();
			if(Char)
			{
				SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
				if(dmg.GetIsUnconscious())
				{
					return false;
				}
				dmg.ForceUnconsciousness();
				dmg.SetResilienceRegenScale(0);
				dmg.AddParticularBleeding();
				dmg.GetOnDamageOverTimeRemoved().Insert(OnCharacterRescued);
				dmg.GetOnDamageStateChanged().Insert(FailTask);
			}
		}
		return true;
	};
}