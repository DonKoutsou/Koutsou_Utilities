modded class DialogueUIClass: ChimeraMenuBase
{ 

	//------------------------------------------------------------------------------------------------//
	override void UpdateEntries(IEntity Character, IEntity Player)
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
			/*
			PlayerManager mngr = GetGame().GetPlayerManager();
			int pid = mngr.GetPlayerIdFromControlledEntity(myUserEntity);
			PlName = mngr.GetPlayerName(pid);
			PLrank = DiagComp.GetCharacterRankName(myUserEntity);
			Plfaction = DiagComp.GetCharacterFaction(myUserEntity).GetFactionKey();
			*/ 
			PlName = DiagComp.GetCharacterName(myUserEntity);
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
		m_sPlRank = PLrank;
		
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
				elComp7.SetTextNumber("[ESC]");
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
				elComp7.SetTextNumber("[ESC]");
			}
		GetGame().GetInputManager().AddActionListener("DialogueBack", EActionTrigger.DOWN, LeaveFunction);
	}
}