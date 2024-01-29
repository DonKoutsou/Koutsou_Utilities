//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_BuildLightPathTask: SP_Task
{
	[Attribute("0",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	ref array <SP_BaseEn> m_aBasesToConnect;
	
	ref Path m_path;
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string OLoc;
		GetInfo(OName, OLoc);
		m_sTaskDesc = "Reopen the paths to neighbouring bases.";
		m_sTaskDiag = string.Format("Set up light posts on the paths leading out of %1 base to %2.", m_aBasesToConnect[0], m_aBasesToConnect[1]);
		m_sTaskTitle = string.Format("Reopen the paths leading out of %1 to %2.", SCR_StringHelper.Translate(SP_BaseNames.Get( m_aBasesToConnect[0] )),SCR_StringHelper.Translate(SP_BaseNames.Get( m_aBasesToConnect[1] )));
		m_sTaskCompletiontext = "Thanks for meeting me %1.";
		m_sacttext = "I'm here to meet you.";
	};
	//------------------------------------------------------------------------------------------------------------//
	//Ready to deliver means package is in assignee's inventory, we are talking to the target and that we are assigned to task
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		array <string> bases = {};
		for (int i; i < m_aBasesToConnect.Count(); i++)
			bases.Insert(SP_BaseNames.Get(m_aBasesToConnect[i]));
		if (!SP_LightPostManager.AreBasesConnected(bases))
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
	override typename GetClassName(){return SP_BuildLightPathTask;};
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignCharacter(IEntity Character)
	{
		if (super.AssignCharacter(Character))
		{
			
			if (m_path)
			{
				SP_LightPostManager.EnableBuildingPreviews(m_path);
				m_path.SpawnTaskMarkers(Character);
			}
			SCR_CampaignMilitaryBaseManager BaseMan = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
			foreach (int basename : m_aBasesToConnect)
			{
				BaseMan.SetBaseVisible(BaseMan.GetNamedBase(SP_BaseNames.Get( basename )));
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
		if (!m_aBasesToConnect)
			m_aBasesToConnect = {};
		//-------------------------------------------------//
		if (m_aBasesToConnect.IsEmpty())
		{
			SCR_CampaignMilitaryBaseManager man = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
			SCR_CampaignMilitaryBaseComponent base = man.GetClosestBase(m_eTaskOwner.GetOrigin());	
			array<string> basesInRange = {};
			array<Path> Paths = {};
			SP_LightPostManager.GetInstane().GetPathsForBase(Paths, base.GetBaseName());
			m_path = Paths.GetRandomElement();
			m_path.GetBases(basesInRange);
			
			if (!basesInRange.IsEmpty())
			{
				foreach (string disbase : basesInRange)
				{
					array <string> bases = {};
					SCR_CampaignMilitaryBaseComponent Dbase = man.GetNamedBase(disbase);
					bases.Insert(Dbase.GetBaseName());
					bases.Insert(base.GetBaseName());
					if (!SP_LightPostManager.AreBasesConnected(bases))
					{
						m_aBasesToConnect.Insert(SP_BaseNames.Convert( Dbase ));
					}
				}
			}
				
		}
		
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