modded class SP_DialogueComponent
{
	string GenerateRummor(IEntity Instigator, IEntity Player)
	{
		if (!GameMode)
			return "Missing GameMode cunt";
		string rummor;
		SP_RequestManagerComponent RequestManager = SP_RequestManagerComponent.Cast(GameMode.FindComponent(SP_RequestManagerComponent));
		CharacterHolder Characters = RequestManager.GetCharacterHolder();
		SP_FactionManager FactionMan = SP_FactionManager.Cast(GetGame().GetFactionManager());
		FactionAffiliationComponent Affiliation = FactionAffiliationComponent.Cast(Instigator.FindComponent(FactionAffiliationComponent));
		if (!FactionMan || !Characters || !RequestManager)
		{
			return STRING_EMPTY;
		}
		int index = Math.RandomInt(0, 5);
		switch (index)
		{
			case 0:
				{
					//look for bounty of friendly unit
				array <ref SP_Task> tasks = new array <ref SP_Task> ();
				RequestManager.GetTasksOfSameType(tasks, SP_BountyTask);
				if (tasks.IsEmpty())
					break;
				foreach (SP_Task task : tasks)
					{
						IEntity target = task.GetTarget();
						if (target == Instigator)
							break;
						FactionAffiliationComponent targetaffiliation = FactionAffiliationComponent.Cast(target.FindComponent(FactionAffiliationComponent));
						if (targetaffiliation.GetAffiliatedFaction() != Affiliation.GetAffiliatedFaction())
							break;
						rummor = string.Format("I heard that someone has put a bounty on %1's head", GetCharacterName(target));
						break;
					}
				}
			break;
			case 1:
				{
					//look for task of friendly unit
					ChimeraCharacter Friendly;
					Characters.GetUnitOfFaction(Affiliation.GetAffiliatedFaction(), Friendly);
					if (Friendly == Instigator && Friendly == Player)
						break;
					array <ref SP_Task> tasks = new array <ref SP_Task> ();
					RequestManager.GetCharTasks(Friendly, tasks);
					if (tasks.IsEmpty())
						break;
					rummor = tasks.GetRandomElement().GetTaskDescription();
				}
			break;
			case 2:
				{
					//look for enemy units and report location
					ChimeraCharacter Enemy;
					array <Faction> enemFactions = new array <Faction>();
					FactionMan.GetEnemyFactions(Affiliation.GetAffiliatedFaction(), enemFactions);
					if (enemFactions.IsEmpty())
						break;
					Faction enemFaction = enemFactions.GetRandomElement();
					if (!Characters.GetUnitOfFaction(enemFaction, Enemy))
						break;
					rummor = string.Format("We have reports of %1 units in %2.", enemFaction.GetFactionKey(), GetCharacterLocation(Enemy));
				}
			break;
			case 3:
				{
					//look for lost groups
					array <ref SP_Task> tasks = new array <ref SP_Task> ();
					RequestManager.GetTasksOfSameType(tasks, SP_RescueTask);
					if (tasks.IsEmpty())
						break;
					foreach (SP_Task Task : tasks)
					{
						SP_RescueTask resctask = SP_RescueTask.Cast(Task);
						IEntity target = Task.GetTarget();
						FactionAffiliationComponent afcomp = FactionAffiliationComponent.Cast(target.FindComponent(FactionAffiliationComponent));
						if (afcomp.GetAffiliatedFaction() == Affiliation.GetAffiliatedFaction())
							{
								string Oname;
								string OLoc;
								resctask.GetInfo(Oname, OLoc);
								rummor = string.Format("I heard about %1's squad lossing contact with HQ around %2. If you are around the area keep an eye out", Oname, OLoc);
								resctask.AssignCharacter(Player);
								break;
							}
					}
				}
			break;
			case 4:
				{
					//Tell about dead friendlies
					ChimeraCharacter deadchar = Characters.GetRandomDeadOfFaction(Affiliation.GetAffiliatedFaction());
					if (deadchar)
					{
						string charname = GetCharacterName(deadchar);
						rummor = string.Format("%1 lost his life not so long ago, I will miss him.", charname);
					}
					break;
				}
			break;
		}
		return rummor;
	}
}