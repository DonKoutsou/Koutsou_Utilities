modded class SP_DialogueComponent
{
	
	void IntroducitonSting(IEntity talker, IEntity Player)
	{
		string IndtroducionString;
		while (!IndtroducionString)
		{
			int index = Math.RandomInt(0,12);
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
					IndtroducionString = ComposeReputationCommentString(Player);
					
				}
				break;
				//tell about a task of his
				case 4:
				{
					IndtroducionString = ComposeAvailableTasksString(talker);
				}
				break;
				//comment on tasks completed by character
				case 5:
				{
					IndtroducionString = ComposeCompletedTasksString(talker, Player);
				}
				break;
				//Rummor
				case 6:
				{
					IndtroducionString = ComposeEnemyWarningString(talker, Player)
				}
				break;
				//dead guy news
				case 7:
				{
					IndtroducionString = ComposeDeadFriendlyString(talker, Player)
				}
				break;
				//Look for tasks of friendlies to share
				case 8:
				{
					IndtroducionString = ComposeFriendlyTaskString(talker, Player)
				}
				break;
				//look for bounties on friendly units
				case 9:
				{
					IndtroducionString = ComposeFriendlyBountyString(talker, Player)
				}
				break;
				//look for lost groups
				case 10:
				{
					IndtroducionString = LookForLostGroups(talker, Player)
				}
				break;
				//Give info about current task being done by ReleaseAI
				case 11:
				{
					IndtroducionString = ComposeCurrentTaskComment(talker);
				}
			}
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
	string LookForLostGroups(IEntity Instigator, IEntity Player)
	{
		//look for lost groups
		string Taskstring;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(Instigator.FindComponent(FactionAffiliationComponent));
		array <ref SP_Task> tasks = new array <ref SP_Task> ();
		SP_RequestManagerComponent.GetTasksOfSameType(tasks, SP_RescueTask);
		if (tasks.IsEmpty())
			return Taskstring;
		foreach (SP_Task Task : tasks)
		{
			SP_RescueTask resctask = SP_RescueTask.Cast(Task);
			IEntity target = Task.GetOwner();
			FactionAffiliationComponent afcomp = FactionAffiliationComponent.Cast(target.FindComponent(FactionAffiliationComponent));
			if (afcomp.GetAffiliatedFaction().IsFactionFriendly(Affiliation.GetAffiliatedFaction()))
				{
					string Oname;
					string OLoc;
					resctask.GetInfo(Oname, OLoc);
					Taskstring = string.Format("I heard about %1's squad lossing contact with HQ around %2. If you are around the area keep an eye out", Oname, OLoc);
					resctask.AssignCharacter(Player);
					break;
				}
		}
		return Taskstring;
	}
	string ComposeFriendlyTaskString(IEntity Instigator, IEntity Player)
	{
		//look for task of friendly unit
		string FriendlyTaskString;
		ChimeraCharacter Friendly;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(Instigator.FindComponent(FactionAffiliationComponent));
		CharacterHolder.GetUnitOfFaction(Affiliation.GetAffiliatedFaction(), Friendly);
		if (Friendly == Instigator && Friendly == Player)
			return FriendlyTaskString;
		array <ref SP_Task> tasks = new array <ref SP_Task> ();
		SP_RequestManagerComponent.GetCharOwnedTasks(Friendly, tasks);
		if (tasks.IsEmpty())
			return FriendlyTaskString;
		FriendlyTaskString = "You arent looking for any work, are you?" + tasks.GetRandomElement().GetTaskDescription();
		return FriendlyTaskString;
	}
	string ComposeFriendlyBountyString(IEntity Instigator, IEntity Player)
	{
		string FriendlyBountyString;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(Instigator.FindComponent(FactionAffiliationComponent));
		array <ref SP_Task> tasks = new array <ref SP_Task> ();
		SP_RequestManagerComponent.GetTasksOfSameType(tasks, SP_BountyTask);
		if (tasks.IsEmpty())
			return FriendlyBountyString;
		foreach (SP_Task task : tasks)
		{
			IEntity target = task.GetTarget();
			if (target == Instigator)
			break;
			FactionAffiliationComponent targetaffiliation = FactionAffiliationComponent.Cast(target.FindComponent(FactionAffiliationComponent));
			if (targetaffiliation.GetAffiliatedFaction() != Affiliation.GetAffiliatedFaction())
			break;
			FriendlyBountyString = string.Format("I heard that someone has put a bounty on %1's head. Keep an eye out for the guy will you?", GetCharacterName(target));
			break;
		}
		return FriendlyBountyString;
	}
	string ComposeEnemyWarningString(IEntity Instigator, IEntity Player)
	{
		//look for enemy units and report location
		string WarningString;
		SCR_FactionManager factman = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(Instigator.FindComponent(FactionAffiliationComponent));
		ChimeraCharacter Enemy;
		array <Faction> enemFactions = new array <Faction>();
		factman.GetEnemyFactions(Affiliation.GetAffiliatedFaction(), enemFactions);
		if (enemFactions.IsEmpty())
			return WarningString;
		Faction enemFaction = enemFactions.GetRandomElement();
		if (!CharacterHolder.GetUnitOfFaction(enemFaction, Enemy))
			return WarningString;
		WarningString = string.Format("We have reports of %1 units in %2. Take care if you are thinking of heading that direction", enemFaction.GetFactionKey(), GetCharacterLocation(Enemy));
		return WarningString;
	}
	string ComposeDeadFriendlyString(IEntity Instigator, IEntity Player)
	{
		string DeadFriendlyString;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(Instigator.FindComponent(FactionAffiliationComponent));
		ChimeraCharacter deadchar = CharacterHolder.GetRandomDeadOfFaction(Affiliation.GetAffiliatedFaction());
		if (deadchar)
		{
			SCR_CharacterIdentityComponent Identitycomp = SCR_CharacterIdentityComponent.Cast(deadchar.FindComponent(SCR_CharacterIdentityComponent));
			int rep = Identitycomp.GetRep();
			string charname = GetCharacterName(deadchar);
			if(rep > 80)
			{
				DeadFriendlyString = string.Format("%1 lost his life not so long ago. We will all miss him, he was a real role model for some of us, such a waste.", charname);
			}
			else if(rep > 60)
			{
				DeadFriendlyString = string.Format("%1 lost his life not so long ago, I will miss him.", charname);
			}
			else if(rep > 40)
			{
				DeadFriendlyString = string.Format("We got some new recently about someone called %1's body was found somewhere on the island, i never knew the guy but maybe you did.", charname);
			}
			else if(rep > 20)
			{
				DeadFriendlyString = string.Format("%1 lost his life not so long ago, didnt have the best reputation around, but cant say i havent seen worse folks in these parts.", charname);
			}
			else if(rep > 1)
			{
				DeadFriendlyString = string.Format("%1 lost his life not so long ago. Going to be honest for the better. That guy was a dick.", charname);
			}
						
		}
		return DeadFriendlyString;
	}
	string ComposeReputationCommentString(IEntity Player)
	{
		SCR_CharacterIdentityComponent Idcomp = SCR_CharacterIdentityComponent.Cast(Player.FindComponent(SCR_CharacterIdentityComponent));
		int MyRep = Idcomp.GetRep();
		string text;
		if(MyRep > 80)
		{
			text = "Your reputetion supercedes you. Keep up the goood work, we need more of oyu here.";
		}
		else if(MyRep > 60)
		{
			text = "I've heard you keep your head down and dont mess around, keep it like that and you wont have to worry about a thing.";
		}
		else if(MyRep > 40)
		{
			text = "Havent heard your name pop up alot. You seem to be finding a way to stay out of peoples mouths. Best kind of reputation is no reputation at all eh?";
		}
		else if(MyRep > 20)
		{
			text = "What do want? Make it quick, dont want to be talking to you more than i have to, you reputation is not the best solider.";
		}
		else if(MyRep > 1)
		{
			text = "Keep your distance, i've heard about you. Dont try anything and this goese well for both.";
		}
		return text;
	}
	string ComposeCompletedTasksString(IEntity talker, IEntity Player)
	{
		array<ref SP_Task> tasks = {};
		string text;
		SP_RequestManagerComponent.GetTasksCompletedBy(Player, tasks);
		FactionAffiliationComponent FactionAff = FactionAffiliationComponent.Cast(talker.FindComponent(FactionAffiliationComponent));
		if (!tasks.IsEmpty())
		{
			SP_Task compltask = tasks.GetRandomElement();
			string task = compltask.GetClassName().ToString();
			if (task == "SP_ChainedTask")
				return text;
			Faction faction = compltask.GetOwnerFaction();
			SCR_Faction CharFaction = SCR_Faction.Cast(FactionAff.GetAffiliatedFaction());
			if (CharFaction == faction)
			{
				text = "I head you've been helping us by providing help to our troops. Every bit is apreciated.";
			}
			else if (CharFaction.IsFactionFriendly(faction))
			{
				text = string.Format("I heard you've been helping the %1 troops by complting tasks for them. Have to keep our friends close.", faction.GetFactionKey());
			}
		}
		return text;
	}
	string ComposeAvailableTasksString(IEntity talker)
	{
		array<ref SP_Task> tasks = {};
		string text;
		SP_RequestManagerComponent.GetCharOwnedTasks(talker, tasks);
		if (!tasks.IsEmpty())
		{
			SP_Task task = tasks.GetRandomElement();
			SP_ChainedTask chtask = SP_ChainedTask.Cast(task);
			if (chtask)
				return text;
			string taskd =  task.GetTaskDiag();
			text = string.Format("You arent looking for any work, are you? %1", taskd);
		}
		return text;
	
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
	string ComposeCurrentTaskComment(IEntity talker)
	{
		string CurrentTaskString;
		AIControlComponent comp = AIControlComponent.Cast(talker.FindComponent(AIControlComponent));
		
		AIAgent agent = comp.GetAIAgent();
			
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		
		SCR_AITaskPickupBehavior action = SCR_AITaskPickupBehavior.Cast(utility.GetCurrentAction());
			
		if (action)
		{
			if (action.PickedTask)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(action.PickedTask.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(action.PickedTask.GetOwner());
				string taskname = action.PickedTask.GetClassName().ToString();
				taskname = taskname.Substring(3, taskname.Length());
				taskname.ToLower();
				CurrentTaskString = string.Format("I'm heading towards %1's location to pick %2", name ,taskname);
			}
		}
		SCR_AIExecuteNavigateTaskBehavior Navaction = SCR_AIExecuteNavigateTaskBehavior.Cast(utility.GetCurrentAction());
			
		if (Navaction)
		{
			if (Navaction.PickedTask)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetTarget());
				string Oname = SP_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetOwner()) + " " + SP_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetOwner());
				
				CurrentTaskString = string.Format("Escorting %1 to %2's location", Oname ,name);
			}
		}
		SCR_AIExecuteDeliveryTaskBehavior Delaction = SCR_AIExecuteDeliveryTaskBehavior.Cast(utility.GetCurrentAction());
		if (Delaction)
		{
			if (Delaction.PickedTask)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(Delaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Delaction.PickedTask.GetTarget());
				CurrentTaskString = string.Format("I am delivering Package to %1", name);
			}
			
		}
		SCR_AIExecuteBountyTaskBehavior Bountyaction = SCR_AIExecuteBountyTaskBehavior.Cast(utility.GetCurrentAction());
		if (Bountyaction)
		{
			if (Bountyaction.PickedTask)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(Bountyaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Bountyaction.PickedTask.GetTarget());
				CurrentTaskString = string.Format("I am going after %1's bounty.", name);
			}
			
		}
		SCR_AIFollowBehavior followact = SCR_AIFollowBehavior.Cast(utility.GetCurrentAction());
		if (followact)
		{
			if (followact.Char)
			{
				string name = SP_DialogueComponent.GetCharacterFirstName(followact.Char) + " " + SP_DialogueComponent.GetCharacterSurname(followact.Char);
				string Tname = SP_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetTarget()) + " " + SP_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetTarget());
				CurrentTaskString = string.Format("I am following %1, he is escorting me to %2's location.", name, Tname);
			}
		}
		if (!CurrentTaskString)
			CurrentTaskString = "I'm relaxing today. Managed to stack some coin so no need to go around doing work for others.";
		return CurrentTaskString;
	}
	string ComposeWeatherComment(IEntity talker)
	{
		TimeAndWeatherManagerEntity TnWman = GetGame().GetTimeAndWeatherManager();
		string weathercomment;
		int index = Math.RandomInt(0,3);
		ref LocalWeatherSituation Weather = new LocalWeatherSituation();
		TnWman.TryGetCompleteLocalWeather(Weather, 0, talker.GetOrigin());
		switch (index)
		{
			//rain
			case 0: 
			{
				if (Weather.GetRainIntensity() > 0.1)
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
				if (Weather.GetFog() > 0.1)
					weathercomment = "Kinda hate this fog, not going to lie";
				else
					weathercomment = "We were expecting some fog today. Maybe later.";
			}
			break;
		}
		return weathercomment;
	}
}
modded class SP_DialogueAction
{
	ref map <IEntity,ref SP_Task> taskstogive = new map <IEntity,ref SP_Task>();
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (GetGame().GetPlayerController().GetControlledEntity() != pUserEntity)
		{
			array <ref SP_Task> taskstodeliver = {};
			SP_RequestManagerComponent.GetReadyToDeliver(pOwnerEntity, taskstodeliver, pUserEntity);
			if (!taskstodeliver.IsEmpty())
			{
				foreach (SP_Task Mytask : taskstodeliver)
				{
					Mytask.CompleteTask(pUserEntity);
				}
			}
			if (taskstogive.Contains(pUserEntity))
			{
				taskstogive.Get(pUserEntity).AssignCharacter(pUserEntity);
				taskstogive.Remove(pUserEntity);
			}
			return;
		}
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