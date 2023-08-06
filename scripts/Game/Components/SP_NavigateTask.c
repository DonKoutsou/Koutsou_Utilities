//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_NavigateTask: SP_Task
{
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
			return true;
		}
		return false;
	};
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
			SP_FactionManager FactionMan = SP_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			if (!CharacterHolder.GetFarUnitOfFaction(ChimeraCharacter.Cast(GetOwner()), 300, Fact, Char))
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
		if (!m_aTaskAssigned.Contains(Assignee))
			return false;
		float dis = vector.Distance(m_eTaskTarget.GetOrigin(), TalkingChar.GetOrigin());
		if(!m_iSuccessDistance)
		{
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			SP_NavigateTask tasksample = SP_NavigateTask.Cast(ReqMan.GetTaskSample(SP_NavigateTask));
			m_iSuccessDistance = tasksample.GetSuccessDistance();
		}
		if(dis <= m_iSuccessDistance)
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
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
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
		m_sTaskDesc = string.Format("%1 is looking for someone to escort him to %2's location.", OName, DName);
		m_sTaskDiag = string.Format("Take me to %1 on %2. Try to keep us away from enemies, what i am carrying cant fall into enemies hands.", DName, DLoc);
		m_sTaskTitle = string.Format("Navigate: escort %1 to %2's location", OName, DName);
		m_sacttext = "I've brought you where you asked.";
	};
	//------------------------------------------------------------------------------------------------------------//
	//to complete task need to assign owner to new group and get rid of WP
	override bool CompleteTask(IEntity Assignee)
	{
		if (GiveReward(Assignee))
		{
			AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			SCR_AIFollowBehavior act = SCR_AIFollowBehavior.Cast(utility.FindActionOfType(SCR_AIFollowBehavior));
			act.SetActiveFollowing(false);
			//utility.SetStateAllActionsOfType(SCR_AIFollowBehavior, EAIActionState.FAILED, false);
			//get group of target
			SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
			if (!dmgman.IsDestroyed())
			{
				AIControlComponent Tcomp = AIControlComponent.Cast(m_eTaskTarget.FindComponent(AIControlComponent));
				AIAgent Tagent = Tcomp.GetAIAgent();
				SCR_AIGroup Tgroup = SCR_AIGroup.Cast(Tagent.GetParentGroup());
				if (Tgroup)
				{
					SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
					group.RemoveAgent(agent);
					//add owner
					Tgroup.AddAgent(agent);
					AIWaypoint wp;
					wp = Tgroup.GetCurrentWaypoint();
					Tgroup.RemoveWaypoint(wp);
					Tgroup.AddWaypoint(wp);
				}
			}
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
	override void AssignCharacter(IEntity Character)
	{
		AIControlComponent comp = AIControlComponent.Cast(m_eTaskOwner.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		SCR_AIFollowBehavior action = new SCR_AIFollowBehavior(utility, null, Character);
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
		group.RemoveAgent(agent);
		Resource groupbase = Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et");
		EntitySpawnParams myparams = EntitySpawnParams();
		myparams.TransformMode = ETransformMode.WORLD;
		myparams.Transform[3] = Character.GetOrigin();
		SCR_AIGroup newgroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupbase, GetGame().GetWorld(), myparams));
		newgroup.AddAgent(agent);
		utility.AddAction(action);
		SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 started to follow you", Diag.GetCharacterName(Character)));
		super.AssignCharacter(Character);
	}
	override string GetCompletionText(IEntity Completionist)
	{
		SP_DialogueComponent diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		string TaskCompletiontext = string.Format("Glad we made it in one piece, thanks alot %1 %2, hope the reward is suficient.", diag.GetCharacterRankName(Completionist), diag.GetCharacterSurname(Completionist));
		return TaskCompletiontext;
	};
	//------------------------------------------------------------------------------------------------------------//
};