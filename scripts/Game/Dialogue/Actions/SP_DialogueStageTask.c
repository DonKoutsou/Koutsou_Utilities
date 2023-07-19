[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTask : DialogueStage
{
	SP_Task t_Task;
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent TaskMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		SP_BountyTask t_Bounty = SP_BountyTask.Cast(t_Task);
		SP_DeliverTask t_Deliver = SP_DeliverTask.Cast(t_Task);
		SP_RetrieveTask t_Retrieve = SP_RetrieveTask.Cast(t_Task);
		SP_RescueTask t_Rescue = SP_RescueTask.Cast(t_Task);
		SP_NavigateTask t_Navigate = SP_NavigateTask.Cast(t_Task);
		if(t_Bounty)
		{
			IEntity BountyPaper = t_Bounty.GetBountyEnt();
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
			InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(BountyPaper.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			invChar.TryRemoveItemFromStorage(BountyPaper, parentSlot.GetStorage());
			if(inv.TryInsertItem(BountyPaper))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("The bounty has been added to your inventory");
			}
			else
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("No space in inventory, bounty left on the floor");
			}
			t_Bounty.AssignCharacter(Player);
		}
		if (t_Deliver)
		{
			IEntity Package = t_Deliver.GetPackage();
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
			InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(Package.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			invChar.TryRemoveItemFromStorage(Package, parentSlot.GetStorage());
			if(inv.TryInsertItem(Package))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("The package has been added to your inventory");
			}
			else
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("No space in inventory, package left on the floor");
			}
			t_Deliver.AssignCharacter(Player);
		}
		if (t_Retrieve)
		{
			IEntity ItemBountyPaper = t_Retrieve.GetItemBountyEnt();
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
			InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(ItemBountyPaper.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			invChar.TryRemoveItemFromStorage(ItemBountyPaper, parentSlot.GetStorage());
			if(inv.TryInsertItem(ItemBountyPaper))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("The item bounty has been added to your inventory");
			}
			else
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("No space in inventory, item bounty left on the floor");
			}
			t_Retrieve.AssignCharacter(Player);
		}
		if(t_Navigate)
		{
			SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
			Diag.Escape(Character, Player);
			AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
			group.RemoveAgent(agent);
			Resource groupbase = Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et");
			EntitySpawnParams myparams = EntitySpawnParams();
			myparams.TransformMode = ETransformMode.WORLD;
			myparams.Transform[3] = Character.GetOrigin();
			SCR_AIGroup newgroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupbase, GetGame().GetWorld(), myparams));
			newgroup.AddAgent(agent);
			SCR_EntityWaypoint m_Waypoint;
			Resource wpRes = Resource.Load("{A0509D3C4DD4475E}prefabs/AI/Waypoints/AIWaypoint_Follow.et");
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = Player.GetOrigin();
			m_Waypoint = SCR_EntityWaypoint.Cast(GetGame().SpawnEntityPrefab(wpRes, GetGame().GetWorld(), params));
			m_Waypoint.SetEntity(SCR_ChimeraCharacter.Cast(Player));
			Player.AddChild(m_Waypoint, -1);
			newgroup.AddWaypointAt(m_Waypoint, 0);
			//utility.AddAction(action);
			t_Navigate.AssignCharacter(Player);
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 started to follow you", Diag.GetCharacterName(Character)));
		}
	};
	void SetupTask(SP_Task task)
	{
		t_Task = task;
	}
	override bool GetActionText(IEntity Character, IEntity Player, out string acttext)
	{
		if(t_Task && t_Task.CharacterAssigned(Player) == false && t_Task.GetState() != ETaskState.COMPLETED)
		{
			acttext = t_Task.GetTaskDescription();
			return true;
		}
		return false;
	}
	override string GetStageDialogueText(IEntity Character, IEntity Player)
		{
			return t_Task.GetTaskDescription();
		 	
		}
}