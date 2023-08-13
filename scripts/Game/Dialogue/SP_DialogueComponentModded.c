modded class SP_DialogueComponent
{
	void IntroducitonSting(IEntity talker, IEntity Player)
	{
		string IndtroducionString;
		int index = Math.RandomInt(0,4);
		switch (index)
		{
			case 0: 
			{
				//Find if the player is lacking sleep/huger/thirst
				SP_CharacterStatsComponent statcomp = SP_CharacterStatsComponent.Cast(GetGame().GetPlayerController().FindComponent(SP_CharacterStatsComponent));
				int reasonindex;
				if (statcomp.MissingSomething(reasonindex))
				{
					IndtroducionString = ComposeNeedString(Player ,reasonindex);
				};
			}
			break;
				//comment on wather
			case 1: 
			{
				IndtroducionString = ComposeWeatherComment(talker);
			}
			break;
				//apreciate equipment
			case 2: 
			{
				CharacterWeaponManagerComponent wepman = CharacterWeaponManagerComponent.Cast(Player.FindComponent(CharacterWeaponManagerComponent));
				BaseWeaponComponent weapon = wepman.GetCurrentWeapon();
				if (weapon)
				{
					IndtroducionString = string.Format("That's a cool %1 you got there.", weapon.GetUIInfo().GetName());
					break;
				}
				EquipedLoadoutStorageComponent m_pLoadout = EquipedLoadoutStorageComponent.Cast(Player.FindComponent(EquipedLoadoutStorageComponent));
				IEntity armoredvest = m_pLoadout.GetClothFromArea(LoadoutArmoredVestSlotArea);
				if (armoredvest)
				{
					InventoryItemComponent item = InventoryItemComponent.Cast(armoredvest.FindComponent(InventoryItemComponent));
					if (item)
					{
						IndtroducionString = string.Format("That's a cool %1 you got there.", item.GetUIInfo().GetName());
						break;
					}
				}
			}
			break;
				//comment on reputation
			case 3: 
			{
				SCR_CharacterIdentityComponent Idcomp = SCR_CharacterIdentityComponent.Cast(Player.FindComponent(SCR_CharacterIdentityComponent));
				int MyRep = Idcomp.GetRep();
				
				if(MyRep > 80)
				{
					IndtroducionString = "Your reputetion supercedes you. Keep up the goood work, we need more of oyu here.";
				}
				else if(MyRep > 60)
				{
					IndtroducionString = "I've heard you keep your head down and dont mess around, keep it like that and you wont have to worry about a thing.";
				}
				else if(MyRep > 40)
				{
					IndtroducionString = "Not heard about you, you new around here?.";
				}
				else if(MyRep > 20)
				{
					IndtroducionString = "What do want? Make it quick, dont want to be talking to you more than i have to.";
				}
				else if(MyRep > 1)
				{
					IndtroducionString = "Keep your distance, dont try anything and this goese well for both.";
				}
			}
			break;
				//
			case 4: 
			{
			
			}
			break;
		}
		TimeAndWeatherManagerEntity TnWman = GetGame().GetTimeAndWeatherManager();
		string Plname = SP_DialogueComponent.GetCharacterSurname(Player);
		string Plrank = SP_DialogueComponent.GetCharacterRankNameFull(Player);
		if (TnWman.GetTimeOfTheDay() > 4.0 && TnWman.GetTimeOfTheDay() < 12.0)
		{
			IndtroducionString = string.Format("Good morning %1 %2. %3", Plrank, Plname, IndtroducionString);
		}
		else
			IndtroducionString = string.Format("Hello %1 %2. %3", Plrank, Plname, IndtroducionString);
		a_texthistory.Insert(IndtroducionString);
	}
	string ComposeNeedString(IEntity Player, int reasonindex)
	{
		string Needstring;
		string Plname = SP_DialogueComponent.GetCharacterSurname(Player);
		switch (reasonindex)
		{
			//temp low
			case 0: 
			{
				Needstring = string.Format("You look like you need some extra clothes on %1.", Plname);
			}
			break;
			//temp high
			case 1: 
			{
				Needstring = string.Format("You are sweating all over %1. Get yourself some water.", Plname);
			}
			break;
			//hunger
			case 2: 
			{
				Needstring = string.Format("I can hear your stomach from all the way here %1. Get something to eat.", Plname);
			}
			break;
			//thirst
			case 3: 
			{
				Needstring = string.Format("You look parched %1. Get yourself some water.", Plname);
			}
			break;
			//energy
			case 4: 
			{
				Needstring = string.Format("Get yourself some sleep %1, you look like shit.", Plname);
			}
			break;
		}
		return Needstring;
	}
	string ComposeWeatherComment(IEntity talker)
	{
		TimeAndWeatherManagerEntity TnWman = GetGame().GetTimeAndWeatherManager();
		string weathercomment;
		int index = Math.RandomInt(0,2);
		switch (index)
		{
			//rain
			case 0: 
			{
				if (TnWman.GetRainIntensity() > 0.1)
					weathercomment = "Kinda hate this rain, not going to lie";
				else
					weathercomment = "We were expecting some rain today. Maybe later, i'm keeping a dry change of clothes just to be sure.";
			}
			//temp
			break;
			case 1: 
			{
				float tmp = TnWman.CalculateFeelsLikeTemperature(talker);
				if (tmp < 15)
				{
					weathercomment = "Getting a bit cold today isnt it?";
				}
				else if (tmp > 25)
				{
					weathercomment = "This heat is verry uncomportable. How you getting by?";
				}
				else if (tmp < 25 && tmp > 15)
				{
					weathercomment = "Temprature has been suprisingly friendly today. Forecast we got was different.";
				}
			}
			//fog
			break;
			case 2: 
			{
			}
			break;
		}
		return weathercomment;
	}
}
modded class SP_DialogueAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		AIControlComponent comp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		SCR_AIConverseBehavior action = new SCR_AIConverseBehavior(utility, null, pUserEntity.GetOrigin());
		
		GameMode = BaseGameMode.Cast(GetGame().GetGameMode());
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		string NoTalkText = "Cant talk to you now";
		FactionKey SenderFaction = DiagComp.GetCharacterFaction(pOwnerEntity).GetFactionKey();
		BaseChatChannel Channel;
		string name = DiagComp.GetCharacterName(pOwnerEntity);
		if (!DiagComp.LocateDialogueArchetype(pOwnerEntity, pUserEntity))
		{
			DiagComp.SendText(NoTalkText, Channel, 0, name, DiagComp.GetCharacterRankName(pOwnerEntity));
			return;
		}
		if (!utility.FindActionOfType(SCR_AIFollowBehavior))
			utility.AddAction(action);
		MenuBase myMenu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.DialogueMenu);
		GetGame().GetInputManager().ActivateContext("DialogueMenuContext");
		DialogueUIClass DiagUI = DialogueUIClass.Cast(myMenu);
		DiagComp.IntroducitonSting(pOwnerEntity, pUserEntity);
		DiagUI.Init(pOwnerEntity, pUserEntity);
		DiagUI.UpdateEntries(pOwnerEntity, pUserEntity);
	}
}