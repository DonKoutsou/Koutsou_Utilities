//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_BuildLightPathTask: SP_Task
{
	[Attribute()]
	ref array <string> m_aBasesToConnect;
	
	ref array <LightPost> a_posts = {};
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string OLoc;
		GetInfo(OName, OLoc);
		m_sTaskDesc = string.Format("Talk to %1.", OName);
		m_sTaskDiag = string.Format("Make your way to %1's location, he's asked to speak with you. He's on %2.", OName, OLoc);
		m_sTaskTitle = string.Format("Talk to %1", OName);
		m_sTaskCompletiontext = "Thanks for meeting me %1.";
		m_sacttext = "I'm here to meet you.";
	};
	//------------------------------------------------------------------------------------------------------------//
	//Ready to deliver means package is in assignee's inventory, we are talking to the target and that we are assigned to task
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (!SP_LightPostManager.AreBasesConnected(m_aBasesToConnect))
		{
			return false;
		}
		
		if (e_State == ETaskState.COMPLETED)
		{
			return false;
		}
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Complete tasks means package is on target's inventory and reward is givven to assigne
	override bool CompleteTask(IEntity Assignee)
	{
		if (GiveReward(Assignee))
		{

			e_State = ETaskState.COMPLETED;
			m_eCopletionist = Assignee;
			if (SCR_EntityHelper.GetPlayer() == Assignee)
				SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
			dmgmn.GetOnDamageStateChanged().Remove(FailTask);
			GetOnTaskFinished(this);
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Info needed for delivery mission is Names of O/T and location names of O/T
	void GetInfo(out string OName, out string OLoc)
	{
		SCR_GameModeCampaign GM = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		if (!GM)
			return;
		DS_DialogueComponent Diag = DS_DialogueComponent.GetInstance();
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = Diag.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_TalkTask;};
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignCharacter(IEntity Character)
	{
		if (super.AssignCharacter(Character))
		{
			SP_LightPostManager.EnableBuildingPreviews(m_aBasesToConnect);
			if (!a_posts.IsEmpty())
			{
				foreach (LightPost post : a_posts)
				{
					post.SpawnTaskMarkers(Character);
				}
			}
			return true;
			
		}
		return false;
	}
	override bool Init()
	{
		if (!m_bPartOfChain)
			InheritFromSample();
		
		//-------------------------------------------------//
		//first look for owner cause targer is usually derived from owner faction/location etc...
		if (!m_eTaskOwner)
		{
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
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		if (!m_aBasesToConnect)
		{
			SCR_CampaignMilitaryBaseManager man = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
			SCR_CampaignMilitaryBaseComponent base = man.GetClosestBase(m_eTaskOwner.GetOrigin());	
			m_aBasesToConnect.Insert(base.GetBaseName());
			array<SCR_CampaignMilitaryBaseComponent> basesInRange = {};
			base.GetBasesInRange(SCR_ECampaignBaseType.BASE, basesInRange);
			if (!basesInRange.IsEmpty())
			{
				foreach (SCR_CampaignMilitaryBaseComponent disbase : basesInRange)
				{
					array <string> bases = {};
					bases.Insert(disbase.GetBaseName());
					bases.Insert(base.GetBaseName());
					if (!SP_LightPostManager.AreBasesConnected(bases))
					{
						m_aBasesToConnect.Insert(disbase.GetBaseName());
					}
				}
			}
				
		}
		SP_LightPostManager.GetLightPolesForBases(m_aBasesToConnect, a_posts);
		
		CreateDescritions();
		AddOwnerInvokers();
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	override int GetTaskType()
	{
		return ETaskType.BUILD;
	}
	
	//------------------------------------------------------------------------------------------------------------//
};
//------------------------------------------------------------------------------------------------------------//