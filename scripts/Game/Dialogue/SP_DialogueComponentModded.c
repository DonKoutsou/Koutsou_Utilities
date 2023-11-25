modded class DS_DialogueComponent
{
	//----------------------------------------------------------------------------------------------------------------//
	static string GetCharacterLocation(IEntity Character, bool unlockloc = 0)
	{
		int m_iGridSizeX;
		int m_iGridSizeY;
		float angleA = 0.775;
		const float angleB = 0.325;
		vector mins,maxs;
		GenericWorldEntity world = GetGame().GetWorldEntity();
		if (!world)
			return STRING_EMPTY;
		world.GetWorldBounds(mins, maxs);
			
		m_iGridSizeX = maxs[0]/3;
		m_iGridSizeY = maxs[2]/3;
	 
		SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
		vector posPlayer = Character.GetOrigin();
		
		SCR_CampaignMilitaryBaseManager BaseMan = SCR_GameModeCampaign.Cast(GameMode).GetBaseManager();
		SCR_CampaignMilitaryBaseComponent nearest = BaseMan.GetClosestBase(Character.GetOrigin());
		if (!nearest)
			return STRING_EMPTY;
		if (unlockloc)
		{
			if (!BaseMan.IsBaseVisible(nearest))
			BaseMan.SetBaseVisible(nearest);
		}
		
		
		GenericEntity nearestLocation = nearest.GetOwner();
		SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(nearestLocation.FindComponent(SCR_MapDescriptorComponent));
		string closestLocationName;
		closestLocationName = nearest.GetBaseName();

		vector lastLocationPos = nearestLocation.GetOrigin();
		float lastDistance = vector.DistanceSqXZ(lastLocationPos, posPlayer);
	
		string closeLocationAzimuth;
		vector result = posPlayer - lastLocationPos;
		result.Normalize();
		
		float angle1 = vector.DotXZ(result,vector.Forward);
		float angle2 = vector.DotXZ(result,vector.Right);
				
		if (angle2 > 0)
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthEast";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionEast";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthEast";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		else
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthWest";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionWest";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthWest";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		int playerGridPositionX = posPlayer[0]/m_iGridSizeX;
		int playerGridPositionY = posPlayer[2]/m_iGridSizeY;
			
		int playerGridID = GetGridIndex(playerGridPositionX,playerGridPositionY);
	 	string m_sLocationName = m_WorldDirections.GetQuadHint(playerGridID) + ", " + closestLocationName;
		return m_sLocationName;
	}
	//used in GetCharacterLocation, duuno
	static protected int GetGridIndex(int x, int y)
	{
		return 3*y + x;
	}
	//function used to generate into dialogue when talking to someone. Should be able to produce multiple lines
	//tbi
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
				//case 9:
				//{
				//	IndtroducionString = ComposeFriendlyBountyString(talker, Player)
				//}
				break;
				//look for lost groups
				case 9:
				{
					IndtroducionString = LookForLostGroups(talker, Player)
				}
				break;
				//Give info about current task being done by ReleaseAI
				case 10:
				{
					IndtroducionString = ComposeCurrentTaskComment(talker);
				}
			}
		}
		ChimeraWorld world = Player.GetWorld();
		if (!world)
			return;
		TimeAndWeatherManagerEntity TnWman = world.GetTimeAndWeatherManager();
		if (!TnWman)
			return;
		string Plname = DS_DialogueComponent.GetCharacterSurname(Player);
		string Plrank = DS_DialogueComponent.GetCharacterRankNameFull(Player);
		if (TnWman.GetTimeOfTheDay() > 4.0 && TnWman.GetTimeOfTheDay() < 12.0)
		{
			IndtroducionString = string.Format("Good morning %1 %2. %3", Plrank, Plname, IndtroducionString);
		}
		else
			IndtroducionString = string.Format("Hello %1 %2. %3", Plrank, Plname, IndtroducionString);
		RegisterCharInHistory(talker);
		a_texthistory.Insert(IndtroducionString);
		array<BaseInfoDisplay> infoDisplays = {};
		GetGame().GetPlayerController().GetHUDManagerComponent().GetInfoDisplays(infoDisplays);
		foreach (BaseInfoDisplay baseInfoDisplays : infoDisplays)
		{
			SCR_IDWidget idw = SCR_IDWidget.Cast(baseInfoDisplays);
			if (idw)
			{
				idw.SetTarget(talker);
				idw.ShowInspectCasualtyWidget(talker);
				continue;
			}
			SCR_DialogueWidget DialogueDisplay = SCR_DialogueWidget.Cast(baseInfoDisplays);
			if (!DialogueDisplay)
				continue;

			DialogueDisplay.SetTarget(talker);
			DialogueDisplay.SetText(IndtroducionString);
			DialogueDisplay.ShowInspectCasualtyWidget(talker);
		}
		a_PLtexthistory.Insert("null");
	}
	string LookForLostGroups(IEntity instigator, IEntity Player)
	{
		//look for lost groups
		string Taskstring;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
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
	string ComposeFriendlyTaskString(IEntity instigator, IEntity Player)
	{
		//look for task of friendly unit
		string FriendlyTaskString;
		ChimeraCharacter Friendly;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
		CharacterHolder.GetUnitOfFaction(Affiliation.GetAffiliatedFaction(), Friendly);
		if (Friendly == instigator && Friendly == Player)
			return FriendlyTaskString;
		array <ref SP_Task> tasks = new array <ref SP_Task> ();
		SP_RequestManagerComponent.GetCharOwnedTasks(Friendly, tasks);
		if (tasks.IsEmpty())
			return FriendlyTaskString;
		FriendlyTaskString = "You arent looking for any work, are you?" + tasks.GetRandomElement().GetTaskDescription();
		return FriendlyTaskString;
	}
	/*string ComposeFriendlyBountyString(IEntity instigator, IEntity Player)
	{
		string FriendlyBountyString;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
		array <ref SP_Task> tasks = new array <ref SP_Task> ();
		SP_RequestManagerComponent.GetTasksOfSameType(tasks, SP_BountyTask);
		if (tasks.IsEmpty())
			return FriendlyBountyString;
		foreach (SP_Task task : tasks)
		{
			IEntity target = task.GetTarget();
			if (target == instigator)
			break;
			FactionAffiliationComponent targetaffiliation = FactionAffiliationComponent.Cast(target.FindComponent(FactionAffiliationComponent));
			if (targetaffiliation.GetAffiliatedFaction() != Affiliation.GetAffiliatedFaction())
			break;
			FriendlyBountyString = string.Format("I heard that someone has put a bounty on %1's head. Keep an eye out for the guy will you?", GetCharacterName(target));
			break;
		}
		return FriendlyBountyString;
	}*/
	string ComposeEnemyWarningString(IEntity instigator, IEntity Player)
	{
		//look for enemy units and report location
		string WarningString;
		SCR_FactionManager factman = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
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
	string ComposeDeadFriendlyString(IEntity instigator, IEntity Player)
	{
		string DeadFriendlyString;
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
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
		string Plname = DS_DialogueComponent.GetCharacterSurname(Player);
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
				string name = DS_DialogueComponent.GetCharacterFirstName(action.PickedTask.GetOwner()) + " " + DS_DialogueComponent.GetCharacterSurname(action.PickedTask.GetOwner());
				string taskname = action.PickedTask.ClassName();
				int length = taskname.Length();
				string tasksubname = taskname.Substring(3, taskname.Length() - 7) + " task";
				tasksubname.ToLower();
				CurrentTaskString = string.Format("I'm heading towards %1's location to pick a %2", name ,tasksubname);
			}
		}
		SCR_AIExecuteNavigateTaskBehavior Navaction = SCR_AIExecuteNavigateTaskBehavior.Cast(utility.GetCurrentAction());
			
		if (Navaction)
		{
			if (Navaction.PickedTask)
			{
				string name = DS_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetTarget()) + " " + DS_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetTarget());
				string Oname = DS_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetOwner()) + " " + DS_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetOwner());
				
				CurrentTaskString = string.Format("Escorting %1 to %2's location", Oname ,name);
			}
		}
		SCR_AIExecuteDeliveryTaskBehavior Delaction = SCR_AIExecuteDeliveryTaskBehavior.Cast(utility.GetCurrentAction());
		if (Delaction)
		{
			if (Delaction.PickedTask)
			{
				string name = DS_DialogueComponent.GetCharacterFirstName(Delaction.PickedTask.GetTarget()) + " " + DS_DialogueComponent.GetCharacterSurname(Delaction.PickedTask.GetTarget());
				CurrentTaskString = string.Format("I am delivering Package to %1", name);
			}
			
		}
		SCR_AIExecuteBountyTaskBehavior Bountyaction = SCR_AIExecuteBountyTaskBehavior.Cast(utility.GetCurrentAction());
		if (Bountyaction)
		{
			if (Bountyaction.PickedTask)
			{
				string name = DS_DialogueComponent.GetCharacterFirstName(Bountyaction.PickedTask.GetTarget()) + " " + DS_DialogueComponent.GetCharacterSurname(Bountyaction.PickedTask.GetTarget());
				CurrentTaskString = string.Format("I am going after %1's bounty.", name);
			}
			
		}
		SCR_AIFollowBehavior followact = SCR_AIFollowBehavior.Cast(utility.GetCurrentAction());
		if (followact)
		{
			if (followact.Char)
			{
				string name = DS_DialogueComponent.GetCharacterFirstName(followact.Char) + " " + DS_DialogueComponent.GetCharacterSurname(followact.Char);
				string Tname = DS_DialogueComponent.GetCharacterFirstName(Navaction.PickedTask.GetTarget()) + " " + DS_DialogueComponent.GetCharacterSurname(Navaction.PickedTask.GetTarget());
				CurrentTaskString = string.Format("I am following %1, he is escorting me to %2's location.", name, Tname);
			}
		}
		if (!CurrentTaskString)
			CurrentTaskString = "I'm relaxing today. Managed to stack some coin so no need to go around doing work for others.";
		return CurrentTaskString;
	}
	string ComposeWeatherComment(IEntity talker)
	{
		ChimeraWorld world = talker.GetWorld();
		TimeAndWeatherManagerEntity TnWman = world.GetTimeAndWeatherManager();
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
modded class DS_DialogueAction
{
	ref map <IEntity,ref SP_Task> taskstogive = new map <IEntity,ref SP_Task>();
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		MenuManager menumanager = GetGame().GetMenuManager();
		ChimeraCharacter Char = ChimeraCharacter.Cast(pOwnerEntity);
		if (Char)
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
			SCR_AIConverseBehavior action = new SCR_AIConverseBehavior(utility, null, pUserEntity, false);
			if (SCR_EntityHelper.IsPlayer(pUserEntity))
			{
				SCR_CharacterControllerComponent cont = SCR_CharacterControllerComponent.Cast(Char.GetCharacterController());
				cont.OnDialogueBegan(pUserEntity, pOwnerEntity);
				DiagComp.AddToContactList(pOwnerEntity);
			}
			/////if not player immitate dialogue and initialise or complete tasks
			else
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
				SCR_AIConverseBehavior Oldaction = SCR_AIConverseBehavior.Cast(utility.FindActionOfType(SCR_AIConverseBehavior));
				if (action)
				{
					action.SetActiveConversation(false);
					DialogueUIClass myMenu = DialogueUIClass.Cast(menumanager.FindMenuByPreset(ChimeraMenuPreset.DialogueMenu));
					if (myMenu && myMenu.myCallerEntity == pOwnerEntity)
					{
						DS_DialogueComponent.GetInstance().Escape(pOwnerEntity ,SCR_EntityHelper.GetPlayer());
					}
						
					vector lookpos = pUserEntity.GetOrigin();
					lookpos[1] = lookpos[1] + 1.70;
					utility.m_LookAction.LookAt(lookpos, 1000);
				}
				else
				{
					vector lookpos = pUserEntity.GetOrigin();
					lookpos[1] = lookpos[1] + 1.70;
					utility.m_LookAction.LookAt(lookpos, 1000);
				}
				return;
			}
			/////
			DiagComp = DS_DialogueComponent.GetInstance();
			string NoTalkText = "Cant talk to you now";
			FactionKey SenderFaction = DiagComp.GetCharacterFaction(pOwnerEntity).GetFactionKey();
			BaseChatChannel Channel;
			string name = DiagComp.GetCharacterName(pOwnerEntity);
			if (!DiagComp.LocateDialogueArchetype(pOwnerEntity, pUserEntity))
			{
				DiagComp.SendText(NoTalkText, Channel, 0, name, DiagComp.GetCharacterRankName(pOwnerEntity));
				return;
			}
			if (!DiagComp.a_PLcontactList.Contains(pOwnerEntity))
					DiagComp.a_PLcontactList.Insert(pOwnerEntity);
			utility.AddAction(action);
			MenuBase myMenu = menumanager.OpenMenu(ChimeraMenuPreset.DialogueMenu);
			GetGame().GetInputManager().ActivateContext("DialogueMenuContext");
			DialogueUIClass DiagUI = DialogueUIClass.Cast(myMenu);
			DiagComp.IntroducitonSting(pOwnerEntity, pUserEntity);
			DiagUI.UpdateEntries(pOwnerEntity, pUserEntity);
		}
		else
		{
			MenuBase myMenu = menumanager.OpenMenu(ChimeraMenuPreset.DialogueMenu);
			GetGame().GetInputManager().ActivateContext("DialogueMenuContext");
			DialogueUIClass DiagUI = DialogueUIClass.Cast(myMenu);
			DiagUI.UpdateEntries(pOwnerEntity, pUserEntity);
		}
	}
	
}