class DialogueUIClass: ChimeraMenuBase
{ 
	//----------------------------------------------------------------//
	//UI Widgets
	protected Widget 							m_wRoot; 
	OverlayWidget 								m_ListBoxOverlay;
	TextWidget 									m_CharacterName;
	TextWidget 									m_PlayerName;
	TextWidget 									m_CharacterRank;
	TextWidget 									m_PlayerRank;
	ImageWidget 								m_CharacterFactionIcon;
	ImageWidget 								m_PlayerFactionIcon;
	PanelWidget									m_CharacterRep;
	PanelWidget									m_PlayerRep;
	ImageWidget 								m_FactionRep;
	SCR_ListBoxElementComponent 				m_ListBoxElement;
    SP_ListBoxComponent 						m_ListBoxComponent;
	
    //----------------------------------------------------------------//
	//PlayerCharacter
	IEntity 									myUserEntity;           
	//----------------------------------------------------------------//
	//Charactaer we are talking to
	IEntity 									myCallerEntity;
	string 										CharName;
	string 										PlName;
	string 										CharRank;
	string 										m_sPlRank;
	string 										rank;
	string		 								PLrank;
	FactionKey 									faction;
	FactionKey 									Plfaction;
	//----------------------------------------------------------------//
	//DialogueStystem
	SP_DialogueComponent 						DiagComp;
	SP_DialogueArchetype 						DiagArch;
	SCR_CharacterIdentityComponent				m_IDComp;
	SCR_CharacterIdentityComponent				m_CharIDComp;
	protected BaseGameMode 					GameMode = BaseGameMode.Cast(GetGame().GetGameMode());
	int 										CurrentBranchID;
    //------------------------------------------------------------------------------------------------//
	override void OnMenuUpdate(float tDelta)
	{
		bool ctxActive = GetGame().GetInputManager().IsContextActive("DialogueMenuContext");
		m_OnTextEditContextChange.Invoke(ctxActive);

		// Invoke OnUpdate
		m_OnUpdate.Invoke(tDelta);
		GetGame().GetInputManager().ActivateContext("DialogueMenuContext");
	}
	void RemoveListeners()
	{
		for (int i = 0; i < m_ListBoxComponent.GetItemCount(); i++)
		{
			GetGame().GetInputManager().RemoveActionListener(string.Format("Dialogue%1", i), EActionTrigger.DOWN, SP_ListBoxElementComponent.Cast(m_ListBoxComponent.GetElementComponent(i)).OnKeyPressed);
		}
		GetGame().GetInputManager().RemoveActionListener("DialogueBack", EActionTrigger.DOWN, DoDialogueBack);
		GetGame().GetInputManager().RemoveActionListener("DialogueBack", EActionTrigger.DOWN, LeaveFunction);
	}
	void Init(IEntity Owner, IEntity User)
	{
		myCallerEntity = Owner;
		myUserEntity = User;
		m_wRoot = GetRootWidget();
		m_ListBoxOverlay = OverlayWidget.Cast(m_wRoot.FindAnyWidget("ListBox0")); 
		m_ListBoxComponent = SP_ListBoxComponent.Cast(m_ListBoxOverlay.FindHandler(SP_ListBoxComponent));
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		m_IDComp = SCR_CharacterIdentityComponent.Cast(User.FindComponent(SCR_CharacterIdentityComponent));
		m_CharIDComp = SCR_CharacterIdentityComponent.Cast(myCallerEntity.FindComponent(SCR_CharacterIdentityComponent));
	}
	//------------------------------------------------------------------------------------------------//
	void UpdateEntries(IEntity Character, IEntity Player)
	{
		Init(Character, Player);
		string DiagText;
		int entryamount;
		if(myCallerEntity)
		{
			CharName = DiagComp.GetCharacterName(myCallerEntity);
			rank = DiagComp.GetCharacterRankName(myCallerEntity);
			faction = DiagComp.GetCharacterFaction(myCallerEntity).GetFactionKey();
		}
		if(myUserEntity)
		{
			PlayerManager mngr = GetGame().GetPlayerManager();
			int pid = mngr.GetPlayerIdFromControlledEntity(myUserEntity);
			PlName = mngr.GetPlayerName(pid);
			PLrank = DiagComp.GetCharacterRankName(myUserEntity);
			Plfaction = DiagComp.GetCharacterFaction(myUserEntity).GetFactionKey(); 
		}
		
		SCR_Faction SCRFact = SCR_Faction.Cast(DiagComp.GetCharacterFaction(myCallerEntity));
		int FactionRep = SCRFact.GetFactionRep(DiagComp.GetCharacterFaction(myUserEntity));
		if(FactionRep > 50)
		{
			m_FactionRep = ImageWidget.Cast(m_wRoot.FindAnyWidget("FactionRep()"));
			m_FactionRep.SetColor(Color.DarkGreen);
		}
		else if(FactionRep <= 50 &&FactionRep > 20)
		{
			m_FactionRep = ImageWidget.Cast(m_wRoot.FindAnyWidget("FactionRep()"));
			m_FactionRep.SetColor(Color.DarkYellow);
		}
		else if(FactionRep <= 20)
		{
			m_FactionRep = ImageWidget.Cast(m_wRoot.FindAnyWidget("FactionRep()"));
			m_FactionRep.SetColor(Color.DarkRed);
		}
		
		CharRank = rank;
		PLrank = m_sPlRank;
		
		AssignIcons(faction, Plfaction);
		
		int MyRep = m_IDComp.GetRep();
		int CharRep = m_CharIDComp.GetRep();
		
		if(MyRep > 50)
		{
			m_PlayerRep = PanelWidget.Cast(m_wRoot.FindAnyWidget("ReputationColor"));
			m_PlayerRep.SetColor(Color.DarkGreen);
		}
		else if(MyRep <= 50 && MyRep > 20)
		{
			m_PlayerRep = PanelWidget.Cast(m_wRoot.FindAnyWidget("ReputationColor"));
			m_PlayerRep.SetColor(Color.DarkYellow);
		}
		else if(MyRep <= 20)
		{
			m_PlayerRep = PanelWidget.Cast(m_wRoot.FindAnyWidget("ReputationColor"));
			m_PlayerRep.SetColor(Color.DarkRed);
		}
		if(CharRep > 50)
		{
			m_CharacterRep = PanelWidget.Cast(m_wRoot.FindAnyWidget("CharReputationColor"));
			m_CharacterRep.SetColor(Color.DarkGreen);
		}
		else if(CharRep <= 50 && CharRep > 20)
		{
			m_CharacterRep = PanelWidget.Cast(m_wRoot.FindAnyWidget("CharReputationColor"));
			m_CharacterRep.SetColor(Color.DarkYellow);
		}
		else if(CharRep <= 20)
		{
			m_CharacterRep = PanelWidget.Cast(m_wRoot.FindAnyWidget("CharReputationColor"));
			m_CharacterRep.SetColor(Color.DarkRed);
		}
		
		m_CharacterName = TextWidget.Cast(m_wRoot.FindAnyWidget("CharacterName"));
		m_CharacterName.SetText(CharName);
		
		m_CharacterRank = TextWidget.Cast(m_wRoot.FindAnyWidget("CharacterRank"));
		m_CharacterRank.SetText(CharRank);
		
		m_PlayerName = TextWidget.Cast(m_wRoot.FindAnyWidget("PlayerName0"));
		m_PlayerName.SetText(PlName);

		m_PlayerRank = TextWidget.Cast(m_wRoot.FindAnyWidget("PlayerRank0"));
		m_PlayerRank.SetText(m_sPlRank);
		
		for (int i = 0; i < 7; i++)
		{
			DiagComp.GetActionName(i, myCallerEntity, Player, DiagText);
			if (DiagText != STRING_EMPTY)
			{
				m_ListBoxComponent.AddItem(DiagText);
				CurrentBranchID = i;
				SP_ListBoxElementComponent elComp = SP_ListBoxElementComponent.Cast(m_ListBoxComponent.GetElementComponent(entryamount));
				elComp.branch = i;
				elComp.m_OnClicked.Insert(ExecuteDialogue);
				string entrynumber = (entryamount + 1).ToString();
				if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
				{
					elComp.SetTextNumber(GetGamepadButtonText(entryamount));
				}
				else
				{
					elComp.SetTextNumber(entrynumber);
				}
				DiagText = STRING_EMPTY;
				GetGame().GetInputManager().AddActionListener(string.Format("Dialogue%1", entryamount.ToString()), EActionTrigger.DOWN, elComp.OnKeyPressed);
				entryamount = entryamount + 1;
			}
		}
		//Check if Archtype is branched an choose to create a Leave button or a Go Back button
		SP_DialogueArchetype DArch = DiagComp.LocateDialogueArchetype(myCallerEntity, Player);
		if (DArch.IsCharacterBranched == true)
		{
			m_ListBoxComponent.AddItem("Go Back");
			SP_ListBoxElementComponent elComp7 = SP_ListBoxElementComponent.Cast(m_ListBoxComponent.GetElementComponent(entryamount));
			elComp7.m_OnClicked.Insert(DoDialogueBack);
			if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp7.SetTextNumber("B");
			}
			else
			{
				elComp7.SetTextNumber("[BACKSPACE]");
			}
			GetGame().GetInputManager().AddActionListener("DialogueBack", EActionTrigger.DOWN, DoDialogueBack);
			return;
		}
		m_ListBoxComponent.AddItem("Leave");
		SP_ListBoxElementComponent elComp7 = SP_ListBoxElementComponent.Cast(m_ListBoxComponent.GetElementComponent(entryamount));
		elComp7.m_OnClicked.Insert(LeaveFunction);
		if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp7.SetTextNumber("B");
			}
			else
			{
				elComp7.SetTextNumber("[BACKSPACE]");
			}
		GetGame().GetInputManager().AddActionListener("DialogueBack", EActionTrigger.DOWN, LeaveFunction);
	}
	//------------------------------------------------------------------------------------------------//
	//Function called to close menu
    void LeaveFunction()
    {
		RemoveListeners();
		DiagComp.DoBackDialogue(myCallerEntity, myUserEntity);
		DiagComp.ReleaseAI(myCallerEntity, myUserEntity);
		GetGame().GetMenuManager().CloseAllMenus();
    }
	override void OnMenuClose()
	{
		RemoveListeners();
	}
	//------------------------------------------------------------------------------------------------//
	void ExecuteDialogue(int num)
	{
		RemoveListeners();
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, num);
	}
	//------------------------------------------------------------------------------------------------//
	void DoDialogueBack()
	{
		RemoveListeners();
		DiagComp.DoBackDialogue(myCallerEntity, myUserEntity);
	}
	//------------------------------------------------------------------------------------------------//
	string GetGamepadButtonText(int index)
	{
	    switch (index)
	    {
	        case 1:
	            return "A";
			break;
	        case 2:
	            return "X";
			break;
	        case 3:
	            return "Y";
			break;
	        case 4:
	            return "D-Pad Up";
			break;
	        case 5:
	            return "D-Pad Down";
			break;
	        case 6:
	            return "D-Pad Left";
			break;
	        case 7:
	            return "D-Pad Right";
			break;
	        default:
	            return STRING_EMPTY;
			break;
	    }
		return STRING_EMPTY;
	}
	void AssignIcons(FactionKey factioncharacter, FactionKey factionplayer)
	{
		m_CharacterFactionIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("FactionIcon"));
		m_PlayerFactionIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("PlayerFactionIcon0"));
		if (m_CharacterFactionIcon)
		{
			switch(factioncharacter)
			{
				case "US":
					m_CharacterFactionIcon.LoadImageTexture(0, "{EB7F65DBC9392557}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_USA.edds");
				break;
				case "USSR":
					m_CharacterFactionIcon.LoadImageTexture(0, "{40B12B0DF911B856}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_USA.edds");
				break;
				case "FIA":
					m_CharacterFactionIcon.LoadImageTexture(0, "{FB266CDD4502D60B}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_Fia.edds");
				break;
				case "BANDITS":
					m_CharacterFactionIcon.LoadImageTexture(0, "{855625FE8D6A87A8}UI/Textures/DamageState/damageState-dead.edds");
				break;
				case "SPEIRA":
					m_CharacterFactionIcon.LoadImageTexture(0, "{CEAC8771342689C5}Assets/Data/Faction_SPEIRA.edds");
				break;	
			}
		}
		if (m_PlayerFactionIcon)
		{
			switch(factionplayer)
			{
				case "US":
					m_PlayerFactionIcon.LoadImageTexture(0, "{EB7F65DBC9392557}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_USA.edds");
				break;
				case "USSR":
					m_PlayerFactionIcon.LoadImageTexture(0, "{40B12B0DF911B856}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_USA.edds");
				break;
				case "FIA":
					m_PlayerFactionIcon.LoadImageTexture(0, "{FB266CDD4502D60B}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_Fia.edds");
				break;
				case "BANDITS":
					m_PlayerFactionIcon.LoadImageTexture(0, "{855625FE8D6A87A8}UI/Textures/DamageState/damageState-dead.edds");
				break;
				case "SPEIRA":
					m_PlayerFactionIcon.LoadImageTexture(0, "{CEAC8771342689C5}Assets/Data/Faction_SPEIRA.edds");
				break;	
			}
		}
	}
}