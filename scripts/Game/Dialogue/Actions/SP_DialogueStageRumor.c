[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageRumor : DialogueStage
{
	ref array <int> usedindex = new array<int>();
	
	override void Perform(IEntity Character, IEntity Player)
	{
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	};
	override string GetStageDialogueText(IEntity Character, IEntity Player)
	{
			FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
			Faction key = FC.GetAffiliatedFaction();
			return GetRandomLocationPopulation(key, Player);
	};
	string GetRandomLocationPopulation(Faction key, IEntity Player)
	{
		int random;
		random = Math.RandomInt(0, 10);
		if (random == 0)
		{
			array<ref SP_Task> rescuetasks = new array<ref SP_Task>();
			SP_RequestManagerComponent req = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			req.GetRescueTasks(rescuetasks);
			foreach (SP_Task Task : rescuetasks)
			{
				SP_RescueTask resctask = SP_RescueTask.Cast(Task);
				IEntity target = Task.GetTarget();
				FactionAffiliationComponent afcomp = FactionAffiliationComponent.Cast(target.FindComponent(FactionAffiliationComponent));
				if (afcomp.GetAffiliatedFaction() == key)
					{
						string Oname;
						string Dname;
						string OLoc;
						string DLoc;
						resctask.GetInfo(Oname, Dname, OLoc, DLoc);
						string TextToSend = string.Format("I heard about %1's squad lossing contact with HQ around %2. If you are around the area keep an eye out", Dname, DLoc);
						resctask.AssignCharacter(Player);
						return TextToSend;
					}
			}
		}
		int index;
		SP_AIDirector RandomDirector;
		index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
		RandomDirector = SP_AIDirector.AllDirectors[index];
		usedindex.Insert(index);
		
		while(usedindex.Contains(index) == true)
		{
			index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			RandomDirector = SP_AIDirector.AllDirectors[index];
		}
		string FactioReadble = "";
		SCR_Faction faction = SCR_Faction.Cast(RandomDirector.GetMajorityHolder(FactioReadble));
		
		while (faction.IsFactionFriendly(key) == true)
		{
			index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			if(usedindex.Contains(index) == false)
			{
				RandomDirector = SP_AIDirector.AllDirectors[index];
				usedindex.Insert(index);
			}
			faction = SCR_Faction.Cast(RandomDirector.GetMajorityHolder(FactioReadble));
		}	
		if(SP_AIDirector.AllDirectors.Count() <= usedindex.Count())
		{
			usedindex.Clear();
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));		
		string TextToSend = string.Format(DialogueText, FactioReadble, Diag.GetCharacterLocation(RandomDirector));
		return TextToSend;
	}

};