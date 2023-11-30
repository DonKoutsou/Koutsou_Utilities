//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_NavigateTask: SP_Task
{
	[Attribute(defvalue: "300", desc: "Min distance for task to be valid")]
	int m_iTargetOwnerMinDistance;
	//----------------------------------------------------------------------------------//
	[Attribute(defvalue: "20", desc: "Distance from target at wich task can be considered ready to deliver")]
	int m_iSuccessDistance;
	//------------------------------------------------------------------------------------------------------------//
	//In navigate task we look for random owner
	override bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
			if (!Char)
			{
				SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
				if (group)
				{
					Char = ChimeraCharacter.Cast(group.GetLeaderEntity());
				}
			}
		}
		else
		{
			if(!CharacterHolder.GetRandomUnit(Char))
				return false;
		}
		if (Char)
			Owner = Char;
		if(Owner)
		{
			//if someone is doing a task, they should be looking to be escorted somewhere.
			if (SP_RequestManagerComponent.GetassignedTaskCount(Owner) > 0)
				return false;
			if (SP_RequestManagerComponent.CharIsPickingTask(Owner))
				return false;
			return true;
		}
		return false;
	};
	override int GetTaskType()
	{
		return ETaskType.NAVIGATE;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Target needs to be from same faction
	override bool FindTarget(out IEntity Target)
	{
		ChimeraCharacter Char;
		if (m_sTaskTargetOverride && GetGame().FindEntity(m_sTaskTargetOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskTargetOverride));
		}
		else
		{
			FactionAffiliationComponent AffiliationComp = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
			SCR_FactionManager FactionMan = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			if (!CharacterHolder.GetFarUnitOfFaction(ChimeraCharacter.Cast(GetOwner()), m_iTargetOwnerMinDistance, Fact, Char))
				return false;
		}
		
		if (Char)
			Target = Char;
		
		if (Target == GetOwner())
			return false;
		
		if(Target)
			return true;
		
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Its ready to deliver when targer and owner are closer than m_iSuccessDistance
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar == m_eTaskTarget)
			return false;
		//if (m_aTaskAssigned != Assignee)
		//return false;
		float dis = vector.Distance(m_eTaskTarget.GetOrigin(), TalkingChar.GetOrigin());
		if(!m_iSuccessDistance)
		{
			SP_NavigateTask tasksample = SP_NavigateTask.Cast(SP_RequestManagerComponent.GetTaskSample(SP_NavigateTask));
			m_iSuccessDistance = tasksample.GetSuccessDistance();
		}
		if(dis <= m_iSuccessDistance + 10)
		{
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
		DS_DialogueComponent Diag = DS_DialogueComponent.GetInstance();
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = CharRank.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);

		if (m_bHasReward && a_Rewards.Get(0))
		{
			string s_RewardName = FilePath.StripPath(a_Rewards.Get(0).GetPrefabData().GetPrefabName());
			s_RewardName = s_RewardName.Substring(0, s_RewardName.Length() - 3);
			s_RewardName.ToLower();
			m_sTaskDesc = string.Format("%1 is looking for someone to escort him to %2. %1 is on %3, go meet him to give you more details if you are interested", OName, DName, OLoc);
			
			m_sTaskDiag = string.Format("I need to go to %1 on %2. I'll give you %3 %4 if you manage to get me there in one piece.", DName, DLoc, a_Rewards.Count(), s_RewardName);
		}
			
		else
			m_sTaskDiag = string.Format("I need to go to %1 on %2.", DName, DLoc);
		m_sTaskTitle = string.Format("Escort %1 to %2's location", OName, DName);
		m_sAcceptTest = string.Format("Follow me. I'll take you to %1's location.", DName);
		m_sacttext = "I've brought you where you asked.";
	};
	//------------------------------------------------------------------------------------------------------------//
	//to complete task need to assign owner to new group and get rid of WP
	override bool CompleteTask(IEntity Assignee)
	{
		//check if assigne is owner meaning owner navigated himself most likely cause last assignee died
		if (Assignee != m_eTaskOwner)
		{
			if (!GiveReward(Assignee))
			{
				return false;
			}
		}
		if (m_TaskMarker)
		{
			m_TaskMarker.Finish(true);
		}
		e_State = ETaskState.COMPLETED;
		m_eCopletionist = Assignee;
		RemoveOwnerInvokers();
		GetOnTaskFinished(this);
		//handle assignee
		UnAssignCharacter();
		//check if task target is alive so we can assign the owner to that goup
		SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!dmgman.IsDestroyed())
		{
			AIControlComponent Tcomp = AIControlComponent.Cast(m_eTaskTarget.FindComponent(AIControlComponent));
			AIAgent Tagent = Tcomp.GetAIAgent();
			SCR_AIGroup Tgroup = SCR_AIGroup.Cast(Tagent.GetParentGroup());
			if (Tgroup)
			{
				AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
				AIAgent agent = comp.GetAIAgent();
				SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
				group.RemoveAgent(agent);
				//add owner
				Tgroup.AddAgent(agent);
				//AIWaypoint wp;
				//wp = Tgroup.GetCurrentWaypoint();
				//Tgroup.RemoveWaypoint(wp);
				//Tgroup.AddWaypoint(wp);
			}
		}
		if (SCR_EntityHelper.GetPlayer() == Assignee)
		{
			SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			return true;
		}
		else
		{
			if (!dmgman.IsDestroyed())
			{
				AIControlComponent Tcomp = AIControlComponent.Cast(m_eTaskTarget.FindComponent(AIControlComponent));
				AIAgent Tagent = Tcomp.GetAIAgent();
				SCR_AIGroup Tgroup = SCR_AIGroup.Cast(Tagent.GetParentGroup());
				if (Tgroup)
				{
					AIWaypoint wp;
					wp = Tgroup.GetCurrentWaypoint();
					AIControlComponent comp = AIControlComponent.Cast(Assignee.FindComponent(AIControlComponent));
					AIAgent agent = comp.GetAIAgent();
					agent.GetParentGroup().AddWaypoint(wp);
				}
				
			}
			return true;
		}
			
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_NavigateTask;};
	//------------------------------------------------------------------------------------------------------------//
	int GetSuccessDistance()
	{
		if (m_iSuccessDistance)
		{
			return m_iSuccessDistance;
		}
		return null;
	};
	override bool AssignCharacter(IEntity Character)
	{
		
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return false;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return false;
		
		//Check if owner is in group with more then 1 and if so make a new one for him
		/*SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
		if (group.GetAgentsCount() > 1)
		{
			group.RemoveAgent(agent);
			Resource groupbase = Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et");
			EntitySpawnParams myparams = EntitySpawnParams();
			myparams.TransformMode = ETransformMode.WORLD;
			myparams.Transform[3] = Character.GetOrigin();
			SCR_AIGroup newgroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupbase, GetGame().GetWorld(), myparams));
			newgroup.AddAgent(agent);
			newgroup.AddWaypoint(group.GetCurrentWaypoint());
		}*/
		if (!super.AssignCharacter(Character))
			return false;
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return false;
		array <ref SP_Task> tasks = {};
		SP_RequestManagerComponent.GetCharOwnedTasks(m_eTaskOwner, tasks);
		if (!tasks.IsEmpty())
		{
			foreach (SP_Task task : tasks)
			{
				if (task.IsOwnerAssigned())
				{
					task.UnAssignOwner();
					break;
				}
			}
		}
		SCR_AIFollowBehavior action = new SCR_AIFollowBehavior(utility, null, m_aTaskAssigned);
		utility.AddAction(action);
		//if player throw popup
		if (GetGame().GetPlayerController().GetControlledEntity() == Character)
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 started to follow you", DS_DialogueComponent.GetCharacterName(Character)));
		}
		//if not player add task action
		else
		{
			AIControlComponent comp2 = AIControlComponent.Cast(m_aTaskAssigned.FindComponent(AIControlComponent));
			if (!comp2)
				return false;
			AIAgent agent2 = comp2.GetAIAgent();
			if (!agent2)
				return false;
			SCR_AIUtilityComponent utility2 = SCR_AIUtilityComponent.Cast(agent2.FindComponent(SCR_AIUtilityComponent));
			if (!utility2)
				return false;
			SCR_AIExecuteNavigateTaskBehavior action2 = new SCR_AIExecuteNavigateTaskBehavior(utility2, null, this);
			if (action2)
				utility2.AddAction(action2);
		}
		AddAssigneeInvokers();
		return true;
	}
	override bool AssignOwner()
	{
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return false;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return false;
		if (!super.AssignOwner())
			return false;
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return false;
		SCR_AIExecuteNavigateTaskBehavior action = new SCR_AIExecuteNavigateTaskBehavior(utility, null, this);
		utility.AddAction(action);
		return true;
	}
	override void UnAssignOwner()
	{
		super.UnAssignOwner();
		if (!m_eTaskOwner)
			return;
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		//Add follow action to owner
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		SCR_AIExecuteNavigateTaskBehavior action = SCR_AIExecuteNavigateTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteNavigateTaskBehavior));
		if (action)
			action.SetActiveFollowing(false);
		
	}
	override string GetCompletionText(IEntity Completionist)
	{
		DS_DialogueComponent diag = DS_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(DS_DialogueComponent));
		string TaskCompletiontext = string.Format("Glad we made it in one piece, thanks alot %1 %2, hope the reward is suficient.", diag.GetCharacterRankName(Completionist), diag.GetCharacterSurname(Completionist));
		return TaskCompletiontext;
	};
	override bool AssignReward()
	{
		
		float dis = vector.Distance(m_eTaskTarget.GetOrigin(), m_eTaskOwner.GetOrigin());
		m_iRewardAmount = m_iRewardAmount * (dis/40);
		if (!super.AssignReward())
			return false;
		return true;
	};
	override void AddTargetInvokers(){};
	override void RemoveTargetInvokers(){};
	override void GetOnOwnerDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveOwnerInvokers();
		if (m_aTaskAssigned)
			UnAssignCharacter();
		FailTask();
		//possible to fail task, if so override dis
	}
	override void GetOnTargetDeath(EDamageState state){};
	override void GetOnAssigneeDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		UnAssignCharacter();
		SetReserved(null);
	}
	override void UnAssignCharacter()
	{
		if (!m_aTaskAssigned)
			return;
		RemoveAssigneeInvokers();
		if (m_aTaskAssigned)
		{
			ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_aTaskAssigned.FindComponent(ScriptedDamageManagerComponent));
			if (!dmgman.IsDestroyed())
			{
				AIControlComponent comp = AIControlComponent.Cast(m_aTaskAssigned.FindComponent(AIControlComponent));
				AIAgent agent = comp.GetAIAgent();
				SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
				SCR_AIExecuteNavigateTaskBehavior act = SCR_AIExecuteNavigateTaskBehavior.Cast(utility.FindActionOfType(SCR_AIExecuteNavigateTaskBehavior));
				if (act)
					act.SetActiveFollowing(false);
			}
		}
		super.UnAssignCharacter();
		ScriptedDamageManagerComponent dmgmanO = ScriptedDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(ScriptedDamageManagerComponent));
		if (!dmgmanO.IsDestroyed())
		{
			AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			SCR_AIFollowBehavior act = SCR_AIFollowBehavior.Cast(utility.FindActionOfType(SCR_AIFollowBehavior));
			if (act)
				act.SetActiveFollowing(false);
		}
	}
	override bool CanBeAssigned(IEntity TalkingChar, IEntity Assignee)
	{
			SCR_ChimeraCharacter Chimera = SCR_ChimeraCharacter.Cast(TalkingChar);
			if (Chimera.IsLookingforWork() || Chimera.IsWorking() || Chimera.IsFollowing())
			{
					return false;
			}
			return true;
	};
	override void InheritFromSample()
	{
		super.InheritFromSample();
		SP_NavigateTask TaskSample = SP_NavigateTask.Cast(SP_RequestManagerComponent.GetTaskSample(GetClassName()));
		if (TaskSample)
		{
			m_iSuccessDistance = TaskSample.m_iSuccessDistance;
			m_iTargetOwnerMinDistance = TaskSample.m_iTargetOwnerMinDistance;
		}
	}
	
	//------------------------------------------------------------------------------------------------------------//
};
class SCR_AIGetNavigateTaskParams : AITaskScripted
{	
	static const string TASK_PORT = "Task";
	static const string TASK_OWNER_PORT		= "TaskOwner";
	static const string TASK_TARGET_PORT				= "TaskTarget";
	static const string TASK_RADIUS_PORT				= "successRadius";
		
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		TASK_PORT
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	//-----------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		TASK_OWNER_PORT,
		TASK_TARGET_PORT,
		TASK_RADIUS_PORT,
	};

	override TStringArray GetVariablesOut()
    {
			//if (!s_aVarsOut.Contains(TASK_RADIUS_PORT))
				//s_aVarsOut.Insert(TASK_RADIUS_PORT);
      return s_aVarsOut;
    }

	//-----------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SP_Task Task;
		if(!GetVariableIn(TASK_PORT, Task))
		{
			NodeError(this, owner, "Invalid Task Provided!");
			return ENodeResult.FAIL;
		}
		SP_NavigateTask navtask = SP_NavigateTask.Cast(Task);
		SetVariableOut(TASK_OWNER_PORT, Task.GetOwner());
		SetVariableOut(TASK_TARGET_PORT, Task.GetTarget());
		if (navtask)
			SetVariableOut(TASK_RADIUS_PORT, navtask.GetSuccessDistance());
		
		return ENodeResult.SUCCESS;
	}	
};
