//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_RescueTask: SP_Task
{
	[Attribute(defvalue: "20")]
	int m_iRewardAverageAmount;
	
	[Attribute(defvalue: "2")]
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
	void OnCharacterRescued(IEntity character)
	{
		if(CharsToRescue.Contains(character))
		{
			CharsToRescue.RemoveItem(character);
		}
	}
	override bool Init()
	{
		CharsToRescue = new ref array <IEntity>();
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		array <ref SP_Task> tasks = new array <ref SP_Task>();
		reqman.GetTasksOfSameType(tasks, SP_RescueTask);
		SP_RescueTask tasksample = SP_RescueTask.Cast(reqman.GetTaskSample(SP_RescueTask));
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
		FindOwner(TaskOwner);
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
		TaskDesc = string.Format("%1's squad was ambussed, they need someone to rescuer them, they are somethwere areound %2", DName, DLoc);
		TaskDiag = string.Format("We havent been able to establish connections with %1's squad for a while, please go to %2 and look for them", DName, DLoc);
		TaskTitle = string.Format("Rescue: locate %1's squad and provide help", DName);
	};
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!TaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		AIControlComponent comp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskTarget.FindComponent(SCR_CharacterRankComponent));
		if(TaskOwner)
		{
			OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
			OLoc = Director.GetCharacterLocation(TaskOwner);
		}
		DName = CharRank.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget);
		DLoc = Director.GetCharacterLocation(TaskTarget);
	};
	override bool FindTarget(out IEntity Target)
	{
		SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
		if (!MyDirector.CreateVictim(Target))
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
			foreach(AIAgent agent : outAgents)
			{
				CharsToRescue.Insert(agent.GetControlledEntity());
			}
			Target = luckygroup.GetLeaderEntity();
			return true;
		}
		return false;
	};
	override bool FindOwner(out IEntity Owner)
	{
		SCR_AIGroup group = SCR_AIGroup.Cast(TaskTarget);
		AIControlComponent comp = AIControlComponent.Cast(GetTarget().FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector MyDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SP_AIDirector NewDir;
		//-----------------------------------------------------------------//
		FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
		string keyunused;
		Faction Fact = MyDirector.GetMajorityHolder(keyunused);
		if (!Fact)
		{
			return false;
		}
		FactionKey key = Fact.GetFactionKey();
		SCR_Faction myfact = SCR_Faction.Cast(factionsMan.GetFactionByKey(key));
		if (!MyDirector.GetDirectorOccupiedByFriendly(myfact, NewDir))
		{
			return false;
		}
		if (!NewDir.GetRandomUnitByFKey(key, Owner))
		{
			return false;
		}
		if (Owner == GetTarget())
		{
			return false;
		}
		if(Owner)
		{
			return true;
		}
		return false;
	};
	override bool AssignReward()
	{
		SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		EEditableEntityLabel RewardLabel;
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
	override void UpdateState()
	{
		foreach(IEntity char : CharsToRescue)
		{
			SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(char.FindComponent(SCR_CharacterDamageManagerComponent));
			if(!dmg.IsDestroyed())
			{
				return;
			}
		}
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
		}
		e_State = ETaskState.FAILED;
	};
	override bool CompleteTask(IEntity Assignee)
	{
		if(CharsToRescue.Count() != 0)
		{
			return false;
		}
		if (GiveReward(Assignee))
		{
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
				}
				e_State = ETaskState.COMPLETED;
				m_Copletionist = Assignee;
				SP_RequestManagerComponent reqman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
				reqman.OnTaskCompleted(this);
				return true;
		}
		return false;
	};
	override typename GetClassName(){return SP_RescueTask;};
}