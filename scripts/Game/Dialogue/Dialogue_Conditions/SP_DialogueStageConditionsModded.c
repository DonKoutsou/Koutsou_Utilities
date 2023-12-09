[BaseContainerProps(configRoot:true)]
class SP_DialogueStageBaseTaskActionCondition : DS_BaseDialogueStageActionCondition
{
	[Attribute(defvalue: "1")]
	bool m_bGetOwnedTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetTargetTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetAssignedTasks;
	[Attribute(defvalue: "1")]
	bool m_bGetReadyToDeliver;
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskType))]
	int m_iGetTasksOfType;
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ETaskState))]
	int m_iGetTasksWithState;
	[Attribute(desc: "Show player tasks")]
	bool m_bUsePlayer;
	
	override bool CanBePerformed(IEntity Character, IEntity Player){};
	void GatherTasks(IEntity Character, IEntity Player, array <ref SP_Task> tasklist)
	{
		SP_RequestManagerComponent Reqman = SP_RequestManagerComponent.GetInstance();
		
		if (m_bGetOwnedTasks)
			Reqman.GetCharOwnedTasks(Character, tasklist);
		if (m_bGetTargetTasks)
			Reqman.GetCharTargetTasks(Character, tasklist);
		if (m_bGetAssignedTasks)
			Reqman.GetassignedTasks(Character, tasklist);
		if (m_bGetReadyToDeliver)
			Reqman.GetReadyToDeliver(Character, tasklist, Player);
		if (m_iGetTasksOfType)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetTaskType() != m_iGetTasksOfType)
					tasklist.Remove(i);
			}
		}
		if (m_iGetTasksWithState)
		{
			for (int i = tasklist.Count() - 1; i >= 0; i--;)
			{
				if (tasklist[i].GetState() != m_iGetTasksWithState)
					tasklist.Remove(i);
			}
		}
	};
}
[BaseContainerProps(configRoot:true)]
class DS_DialogueStageFactionPapersItemCheckActionCondition : DS_BaseDialogueStageActionCondition
{
	[Attribute(defvalue: "1", desc : "if set to false character will be checked instead of player")]
	bool m_bCheckPlayer
	
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck;
		if (m_bCheckPlayer){CharToCheck = Player;}
		else CharToCheck = Character;
		
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(CharToCheck.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;
		
		FactionAffiliationComponent FactAff = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_CampaignFaction fact = SCR_CampaignFaction.Cast(FactAff.GetAffiliatedFaction());
		ResourceName WantedItem = fact.m_FactionPapersPrefab;
		
		DS_PrefabResource_Predicate pred = new DS_PrefabResource_Predicate(WantedItem);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		
		if (entitiesToDrop.IsEmpty())
		{
			if (m_bCheckPlayer)
				m_sCantBePerformedReason = string.Format("  [Missing X%1] ", FilePath.StripPath( WantedItem ));
			else
			{
				string Name = DS_DialogueComponent.GetCharacterName(Character);
				m_sCantBePerformedReason = string.Format("  [%1 is missing X%2 ] ", Name , WantedItem);
			}
				
			return false;
		}		
		return true;
	}
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageAvailableTaskActionCondition : SP_DialogueStageBaseTaskActionCondition
{
	[Attribute()]
	bool m_bUseIndex;
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		}
		array <ref SP_Task> tasklist = {};
		GatherTasks(CharToCheck, CharToAssign, tasklist);
		if (m_bUseIndex)
		{
			if (m_iIndex >= tasklist.Count())
				return false;
		}
		else
			if (tasklist.IsEmpty())
				return false;
		return true;
	};
}
[BaseContainerProps(configRoot:true)]
class SP_DialogueStageCanTaskBeAssignedActionCondition : SP_DialogueStageBaseTaskActionCondition
{
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		IEntity CharToCheck = Character;
		IEntity CharToAssign = Player;
		if (m_bUsePlayer)
		{
			CharToCheck = Player;
			CharToAssign = Character;
		}
		array <ref SP_Task> tasklist = {};
		
		GatherTasks(CharToCheck, CharToAssign, tasklist);
		
		if (m_iIndex >= tasklist.Count())
			return false;
		if (tasklist[m_iIndex].CanBeAssigned(CharToCheck, CharToAssign))
			return true;
		
		return false;
	};
}
