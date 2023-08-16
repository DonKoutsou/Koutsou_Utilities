[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_BountyTask: SP_Task
{
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
		}
		else
		{
			if(!CharacterHolder.GetRandomUnit(Char))
				return false;
		}
		
		if (Char)
			Owner = Char;
		if(Owner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool FindTarget(out IEntity Target)
	{
		ChimeraCharacter Char;
		if (m_sTaskTargetOverride && GetGame().FindEntity(m_sTaskTargetOverride))
		{
			Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskTargetOverride));
		}
		else
		{
			FactionAffiliationComponent AffiliationComp = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
			SCR_FactionManager FactionMan = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			Faction Fact = AffiliationComp.GetAffiliatedFaction();
			if (!Fact)
				return false;
	
			array <Faction> enemies = new array <Faction>();
			FactionMan.GetEnemyFactions(Fact, enemies);
			if (enemies.IsEmpty())
				return false;
			
			if (!CharacterHolder.GetFarUnitOfFaction(ChimeraCharacter.Cast(GetOwner()), 300, enemies.GetRandomElement(), Char))
				return false;
		}
		
		if (Char)
			Target = Char;
		
		if (Target == GetOwner())
			return false;
		
		if(Target)
			return true;
		
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);
		string s_RewardName = FilePath.StripPath(m_Reward);
		s_RewardName = s_RewardName.Substring(0, s_RewardName.Length() - 3);
		s_RewardName.ToLower();
		m_sTaskDesc = string.Format("%1 has put a bounty on %2's head. %1 is on %3, go meet him to give you more details if you are interested", OName, DName, OLoc);
		m_sTaskDiag = string.Format("I've put a bounty on %1's head, last i heard he was located on %2, get me his dogtags and i'll make it worth your while. Reward is %3 %4", DName, DLoc, m_iRewardAmount, s_RewardName);
		m_sTaskTitle = string.Format("Bounty: retrieve %1's dogtags", DName);
		m_sTaskCompletiontext = "Thanks the completing the task %1, he got what he deserved, dont have any regrets on that.";
		m_sAcceptTest = string.Format("Give me %1's bounty.", DName);
		m_sacttext = string.Format("The bounty on %1 is complete.", DName);
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if(!CharacterAssigned(Assignee))
		{
			return false;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterDamageManagerComponent dmgman = SCR_CharacterDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (dmgman.IsDestroyed())
			return true;
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		inv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			return true;
		}
		return false;			
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool CompleteTask(IEntity Assignee)
	{
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Ownerinv = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		Assigneeinv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundTags[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				Assigneeinv.TryRemoveItemFromStorage(FoundTags[0],parentSlot.GetStorage());
				Ownerinv.TryInsertItem(FoundTags[0]);
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
				}
				e_State = ETaskState.COMPLETED;
				m_eCopletionist = Assignee;
				SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
				SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
				dmgmn.GetOnDamageStateChanged().Remove(FailTask);
				GetOnTaskFinished(this);
				return true;
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!m_eTaskOwner || !m_eTaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(m_eTaskOwner) + " " + Diag.GetCharacterName(m_eTaskOwner);
		DName = CharRank.GetCharacterRankName(m_eTaskTarget) + " " + Diag.GetCharacterName(m_eTaskTarget);
		OLoc = Diag.GetCharacterLocation(m_eTaskOwner);
		DLoc = Diag.GetCharacterLocation(m_eTaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//

	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_BountyTask;};
	//------------------------------------------------------------------------------------------------------------//
};
//------------------------------------------------------------------------------------------------------------//
class SP_NamedTagPredicate : InventorySearchPredicate
{
	string m_OwnerName;
	//------------------------------------------------------------------------------------------------------------//
	void SP_NamedTagPredicate(string Name){m_OwnerName = Name;};
	//------------------------------------------------------------------------------------------------------------//
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		DogTagEntity tag = DogTagEntity.Cast(item);
		if (tag)
		{
			string TagOwnerName;
			tag.GetCname(TagOwnerName);
			if (TagOwnerName == m_OwnerName)
			{
				return true;
			}

		}
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------//