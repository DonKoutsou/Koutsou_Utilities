
//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true), TaskAttribute()]
class SP_Task
{
	//-------------------------------------------------//
	[Attribute(defvalue : "1", desc : "Dissabled wont be randomly spawned but still exist as samples")]
	bool m_bEnabled;
	//-------------------------------------------------//
	[Attribute(defvalue : "1", desc : "Can be assigned to AI")]
	bool m_bAssignable;
	//-------------------------------------------------//
	[Attribute(defvalue : "5", desc : "Amount of rep + when completing this task")]
	int m_iRepReward;
	//-------------------------------------------------//
	[Attribute(desc : "Override with entity name in the world wich will become task owner instead of looking for an owner")]
	string m_sTaskOwnerOverride;
	//-------------------------------------------------//
	[Attribute(desc : "Override with entity name in the world wich will become task target instead of looking for an target")]
	string m_sTaskTargetOverride;
	//-------------------------------------------------//
	[Attribute("0", UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(ERequestRewardItemDesctiptor))]
	ERequestRewardItemDesctiptor e_RewardLabel;
	//-------------------------------------------------//
	[Attribute(defvalue: "10", desc: "Reward amount if reward end up being currency, keep at 1 for Distance based tasks (Navigate, Deliver), reward is calculated based on distance and multiplied to this value, so increase to increase multiplier")]
	int m_iRewardAverageAmount;
	//-------------------------------------------------//
	[Attribute(defvalue: "-1", desc: "ammount of time the task will need to be completed in before penalties start appling. In Hours. If set to -1, no limit will be set")]
	float m_fTimeLimit;
	[Attribute(defvalue : "0", desc : "Should Fail On Limit reaching 0")]
	bool m_bFailOnTimeLimit;
	
	//-------------------------------------------------//
	//Character wich created the task
	IEntity m_eTaskOwner;
	//-------------------------------------------------//
	//Target of the task (kill task, deliver task etc... not necesarry on retrieve task)
	IEntity m_eTaskTarget;
	//-------------------------------------------------//
	// task description, should be composed in 3rd person. "Private .. is looking for .... in....."
	string m_sTaskDesc;
	//-------------------------------------------------//
	// task dialogue character will say when he is describing the task so the player can pick it.
	string m_sTaskDiag;
	//-------------------------------------------------//
	// used for UI popups
	string m_sTaskTitle;
	//-------------------------------------------------//
	//Dialogue line for completing the task
	string m_sTaskCompletiontext;
	//-------------------------------------------------//
	//action text for dialogue menu
	string m_sacttext;
	//-------------------------------------------------//
	//action text to show up for player to accept task
	string m_sAcceptTest;
	//-------------------------------------------------//
	//Faction of owner of task
	SCR_Faction m_OwnerFaction;
	//-------------------------------------------------//
	//Reward that is going to be handed to completionist *m_iRewardAmount
	ref array <IEntity> a_Rewards = {};
	//Used for ClearTasks function in SP_DialogueComponent used to clean completed/failed tasks from main task array
	private bool m_bMarkedForRemoval;
	//-------------------------------------------------//
	//Amount of the m_Reward resource that is going to be given to completionist. Calculated average using m_iRewardAverageAmount taken from task sample in SP_RequestManagerComponent
	int m_iRewardAmount;
	//-------------------------------------------------//
	ref TaskDayTimeInfo m_TaskTimeInfo;
	//-------------------------------------------------//
	// used for when an ai has started heading towards taking a task to avoid multiple AI going for same task
	IEntity reserved;
	//-------------------------------------------------//
	//Stato of task using ETaskState enum
	protected ETaskState e_State = ETaskState.EMPTY;
	//-------------------------------------------------//
	//Characters assigned to this task
	IEntity m_aTaskAssigned;
	bool m_bOwnerAssigned;
	bool m_bPartOfChain;
	bool m_bHasReward;
	//-------------------------------------------------//
	//Character that completed this task. Filled after task is complete
	IEntity m_eCopletionist;
	//-------------------------------------------------//
	//task marker
	SP_BaseTask m_TaskMarker;
	//-------------------------------------------------//
	//Invoker for task finished
	private ref ScriptInvoker s_OnTaskFinished = new ref ScriptInvoker();
	//------------------------------------------------------------------------------------------------------------//
	//Owner of task.
	IEntity GetOwner(){return m_eTaskOwner;};
	Faction GetOwnerFaction(){return m_OwnerFaction;};
	//------------------------------------------------------------------------------------------------------------//
	//Task target
	IEntity GetTarget(){return m_eTaskTarget;};
	IEntity GetAssignee(){return m_aTaskAssigned;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetCompletionist(){return m_eCopletionist;};
	//------------------------------------------------------------------------------------------------------------//
	//adds the completioninst's name to the m_sTaskCompletiontext
	string GetCompletionText(IEntity Completionist){return string.Format(m_sTaskCompletiontext, SP_DialogueComponent.GetCharacterRankName(Completionist) + " " + SP_DialogueComponent.GetCharacterSurname(Completionist));};
	//------------------------------------------------------------------------------------------------------------//
	//Returns state of task
	ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	//Returns task descritption
	string GetTaskDescription(){return m_sTaskDesc;}
	//------------------------------------------------------------------------------------------------------------//
	//Returns dialogue of task owner when giving task
	string GetTaskDiag()
	{
		if (GetTimeLimit() != -1)
		{
			return m_sTaskDiag + string.Format(" I need it done within the next %1 hours.", GetTimeLimit().ToString().ToInt());
		}
		else
		{
			return m_sTaskDiag;
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//Return task title
	string GetTaskTitle(){return m_sTaskTitle;}
	//------------------------------------------------------------------------------------------------------------//
	//Get test for action in dialogue menu
	string GetActionText(){return m_sacttext;}
	//------------------------------------------------------------------------------------------------------------//
	//Text charactre will say once task is complete
	string GetAcceptText(){return m_sAcceptTest;}
	//------------------------------------------------------------------------------------------------------------//
	//Getter for task finish invoker
	ScriptInvoker OnTaskFinished(){return s_OnTaskFinished;}
	//------------------------------------------------------------------------------------------------------------//
	event void GetOnTaskFinished(SP_Task Task){if (m_eTaskOwner) RemoveOwnerInvokers(); if (m_eTaskTarget) RemoveTargetInvokers(); if (m_aTaskAssigned) RemoveAssigneeInvokers(); OnTaskFinished().Invoke(Task);};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to delete excess stuff when a task is failed or couldnt initialise
	void DeleteLeftovers(){};
	//------------------------------------------------------------------------------------------------------------//
	bool MarkedForRemoval(){return m_bMarkedForRemoval;};
	//Inherit data from sample
	void InheritFromSample()
	{
		SP_Task tasksample = SP_RequestManagerComponent.GetTaskSample(GetClassName());
		if(!tasksample)
		{
			return;
		}
		//Get reward label
		e_RewardLabel = tasksample.GetRewardLabel();
		
		//Reputation reward
		m_iRepReward = tasksample.GetRepReward();
		
		//Rewardammount
		m_iRewardAverageAmount = tasksample.GetRewardAverage();
		//Average reward ammount
		if(m_iRewardAverageAmount)
		{
			m_iRewardAmount = Math.Ceil(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
		}
		else
		{
			m_iRewardAmount = Math.RandomInt(m_iRewardAverageAmount/2, m_iRewardAverageAmount * 1.5)
		}
		//Time limit
		m_fTimeLimit = tasksample.m_fTimeLimit;
		m_bFailOnTimeLimit = tasksample.m_bFailOnTimeLimit;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to set up all the texts of the task on Init
	void CreateDescritions(){};
	//-------------------------------------//
	//Getter for reputation reward of task
	int GetRepReward(){return m_iRepReward;};
	//-------------------------------------//
	//------------------------------------------------------------------------------------------------------------//
	//Function to set up entities used for the task. E.g. to spawn package on a delivery mission
	bool SetupTaskEntity(){return true;};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to check if task is ready to deliver. Simmilar to CanBePerformed
	bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee){return true;};
	//------------------------------------------------------------------------------------------------------------//
	//Function to test if someone can take that task
	bool CanBeAssigned(IEntity TalkingChar, IEntity Assignee)
	{
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Function to hold logic of what owner should be for each task
	bool FindOwner(out IEntity Owner)
	{
		ChimeraCharacter Char;
		if (m_bPartOfChain)
		{
			if (m_sTaskOwnerOverride && GetGame().FindEntity(m_sTaskOwnerOverride))
			{
				Char = ChimeraCharacter.Cast(GetGame().FindEntity(m_sTaskOwnerOverride));
			}
			else
				return false;
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
	//Function to hold logic of what tarket should be for each task
	bool FindTarget(out IEntity Target){return true;};
	//------------------------------------------------------------------------------------------------------------//
	//Returns class name of task
	typename GetClassName(){return SP_Task;}
	//------------------------------------------------------------------------------------------------------------//
	//Function used to do various checks on owner character found durring initialisation
	bool CheckOwner()
	{
		//if no owner fail
		if (!m_eTaskOwner)
			return false;
		
		//Get Chimera and check if its important character. Return true if it is
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast( m_eTaskOwner );
		if (Char.IsImportantCharacter)
		{
			return true;	
		}
		
		//if owner is player fail
		if ( m_eTaskOwner == SCR_EntityHelper.GetPlayer() )
			return false;
		
		//if dead Fail
		//Get damage manager
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast( m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent ) );
		if (!dmg)
			return false;
		
		//check if destroyed
		if ( dmg.IsDestroyed() )
			return false;
		
		//Check if character has tasks assigned and fail if true
		array<ref SP_Task> assignedtasks = {};
		SP_RequestManagerComponent.GetassignedTasks( m_eTaskOwner, assignedtasks );
		if ( ! assignedtasks.IsEmpty() )
			return false;
		
		
		//Check if char can get more tasks. Should get obsolete once all tasks come from needs.
		/*array<ref SP_Task> tasks = new array<ref SP_Task>();
		SP_RequestManagerComponent.GetCharOwnedTasks(m_eTaskOwner, tasks);
		if(tasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksPerCharacter())
		{
			return false;
		}*/
		
		//Check if char can get more tasks of same type. Should get obsolete once all tasks come from needs.
		array<ref SP_Task> sametasks = {};
		SP_RequestManagerComponent.GetCharTasksOfSameType( m_eTaskOwner , sametasks , GetClassName() );
		if( sametasks.Count() >= SP_RequestManagerComponent.GetInstance().GetTasksOfSameTypePerCharacter() )
		{
			return false;
		}
		/*Faction senderFaction = SP_DialogueComponent.GetCharacterFaction(m_eTaskOwner);
		if (!senderFaction)
			return false;
		m_OwnerFaction = SCR_Faction.Cast(senderFaction);
		if (m_OwnerFaction.GetFactionKey() == "RENEGADE")
		{
			return false;
		};*/
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Function used to do various checks on target character found durring initialisation
	bool CheckTarget()
	{
		//if no owner fail
		if (!m_eTaskTarget)
			return false;
		
		//Get Chimera and check if its important character. Return true if it is
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(m_eTaskTarget);
		if (Char.IsImportantCharacter)
		{
			return true;	
		}
		
		//if target is player fail
		if (m_eTaskTarget == SCR_EntityHelper.GetPlayer())
			return false;
		
		//if dead Fail
		//Get damage manager
		ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		if (!dmg)
			return false;
		
		//check if destroyed
		if (dmg.IsDestroyed())
			return false;
		
		return true;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character provided in the owner of this task
	bool CharacterIsOwner(IEntity Character)
	{
		if (Character == m_eTaskOwner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character provided is target of this task
	bool CharacterIsTarget(IEntity Character)
	{
		if (Character == m_eTaskTarget)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Checks if character is assigned
	bool CharacterAssigned(IEntity Character)
	{
		if(m_aTaskAssigned == Character)
		{
			return true;
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------------------//
	//Manipulation of task state and results
	bool CompleteTask(IEntity Assignee)
	{
		//if giving reward is unsuccesfull fail completion
		if (GiveReward(Assignee))
		{
			//if player task complete marker
			if (m_TaskMarker)
			{
				m_TaskMarker.Finish(true);
			}
			//set state
			e_State = ETaskState.COMPLETED;
			
			//set completionist
			m_eCopletionist = Assignee;
			
			//if player create popup
			if (SCR_EntityHelper.GetPlayer() == Assignee)
				SCR_PopUpNotification.GetInstance().PopupMsg("Completed", text2: m_sTaskTitle);
			
			//invoker
			GetOnTaskFinished(this);
			
			//mark for removal
			m_bMarkedForRemoval = 1;
			
			//unassign character. used to update behavior of AI
			if (m_aTaskAssigned)
				UnAssignCharacter();
			
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void FailTask()
	{
		//if player task complete marker
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("%1 has died, task failed", SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
		}
		
		//check state and unassign char if needed
		if (GetState() == ETaskState.ASSIGNED)
			UnAssignCharacter();
		
		//update state
		e_State = ETaskState.FAILED;
		
		// mark for removal
		m_bMarkedForRemoval = 1;
		
		//make sure to clear behaviors of AI comming to pick up task
		ClearReserves();
		
		//invoker
		GetOnTaskFinished(this);
	}
	//------------------------------------------------------------------------------------------------------------//
	//Fail task duplicate used for stuff other than character dying
	void CancelTask()
	{
		if (m_TaskMarker)
		{
			m_TaskMarker.Fail(true);
			m_TaskMarker.RemoveAllAssignees();
			m_TaskMarker.Finish(true);
			SCR_PopUpNotification.GetInstance().PopupMsg("Failed", text2: string.Format("Faction relations have shifted and %1 has withdrawn his task.", SP_DialogueComponent.GetCharacterName(m_eTaskOwner)));
		}
		e_State = ETaskState.FAILED;
		m_bMarkedForRemoval = 1;
		if (m_aTaskAssigned)
			UnAssignCharacter();
		ClearReserves();
		GetOnTaskFinished(this);
	}
	///////////////////REWARD//////////////
	//------------------------------------------------------------------------------------------------------------//
	//Function used durring init to assign the rewards of the task
	bool AssignReward()
	{
		if (!m_iRewardAmount)
		{
			m_iRewardAmount = Math.Ceil(Math.RandomFloat(m_iRewardAverageAmount * 0.75, m_iRewardAverageAmount * 1.25));			
		}
		//Get catalog
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType( EEntityCatalogType.REQUEST );
		
		//Get items with label
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(e_RewardLabel, null, Mylist);
		
		
		//Figure out if character has reward
		array <IEntity > items = {};
		//Get char inventory
		InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(InventoryStorageManagerComponent));
		
		//make sure that character has items needed for reward if part of chained task
		if (m_bPartOfChain && e_RewardLabel == ERequestRewardItemDesctiptor.CURRENCY)
		{
			SCR_ChimeraCharacter Character = SCR_ChimeraCharacter.Cast(m_eTaskOwner);
			WalletEntity wallet = Character.GetWallet();
			wallet.SpawnCurrency(m_iRewardAmount);
		}
		else if (m_bPartOfChain)
		{
			int acworth;
			array <SCR_EntityCatalogEntry> acentries = {};
			
			foreach ( SCR_EntityCatalogEntry entry : Mylist )
			{
				SP_RequestData data = SP_RequestData.Cast(entry.GetEntityDataOfType(SP_RequestData));
				if (acworth < m_iRewardAmount)
				{
					acworth += data.GetWorth();
					acentries.Insert(entry);
				}
				
				if (data.GetWorth() > m_iRewardAmount)
				{
					acentries.Clear();
					if (TargetInv.TrySpawnPrefabToStorage(entry.GetPrefab()))
						break;
					
				}
			}
			foreach ( SCR_EntityCatalogEntry entry : acentries )
			{
				TargetInv.TrySpawnPrefabToStorage(entry.GetPrefab());
			}
		}
		int rewardworth = m_iRewardAmount;
		//itterate though list to see wich items character has;
		foreach ( SCR_EntityCatalogEntry entry : Mylist )
		{
			//array for found items
			array <IEntity > founditems = {};
			
			// setup of prefab predicate, add prefab name to it
			SCR_PrefabNamePredicate pref = new SCR_PrefabNamePredicate();
			pref.prefabName = entry.GetPrefab();
			
			//Look for items
			TargetInv.FindItems(founditems, pref, EStoragePurpose.PURPOSE_ANY);
			
			//itterate though them and substract worth from rewardworth int. Once rewardworth = 0 break out, meaning we found enough items to fulfill reward
			for (int i = founditems.Count() - 1; i >= 0; i--)
			{
				//Get entry data
				SP_RequestData Data = SP_RequestData.Cast( entry.GetEntityDataOfType( SP_RequestData ) );
				
				if (Data.GetWorth() > m_iRewardAmount)
				{
					items.Clear();
					items.Insert( founditems.Get( i ) );
					rewardworth = 0;
					break;
				}
				
				//substract worth from rewardworth
				rewardworth -= Data.GetWorth();
				
				//Insert in found item array
				items.Insert( founditems.Get( i ) );
				
				//check if rewardworth reached 0 to stop looking though items 
				if (rewardworth <= 0)
				{
					break;
				}
			}
			if (rewardworth == 0)
				break;
		}
		//if character cannot fulfill reward return false;
		if ( rewardworth > 0 )
		{
			Print("Character doesent have enough to fulfill reward for task. Task creationg canceled");
			return false;
		}
		
		//assign rewards to array
		a_Rewards.Copy(items);
		
		//enable rewards
		m_bHasReward = true;
		return true;
	};
	//Setter for time limit of task takes value from Attribute
	void SetTimeLimit()
	{
		// -1 value is used as an off switch
		if (m_fTimeLimit == -1.0)
			return;
		// Average time limit
		float TimeLimit = Math.RandomFloat(m_fTimeLimit/2, m_fTimeLimit * 1.5);
		//setup time container
		m_TaskTimeInfo = TaskDayTimeInfo.FromPointInFuture(TimeLimit, ETaskTimeLimmit.HOURS);
	}
	//used to get time left
	float GetTimeLimit()
	{
		if (!m_TaskTimeInfo)
			return -1;
		TaskDayTimeInfo info = TaskDayTimeInfo.FromTimeOfTheDay();
		float limit = m_TaskTimeInfo.CalculateTimeDifferance(info);
		return limit;
	}
	//------------------------------------------------------------------------------------------------------------//
	//Called when task if completed to give rewards to completionist
	bool GiveReward(IEntity Target)
	{
		//if no reward present return true;
		if (!m_bHasReward)
		{
			return true;
		}
		//if Target is owner no need to do transaction
		if (Target == m_eTaskOwner)
			return true;
		// special logic for handling currency
		if (e_RewardLabel == ERequestRewardItemDesctiptor.CURRENCY)
		{
			//get character wallets
			SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(Target);
			SCR_ChimeraCharacter OChar = SCR_ChimeraCharacter.Cast(m_eTaskOwner);
			WalletEntity wallet = Char.GetWallet();
			WalletEntity Owallet = OChar.GetWallet();
			
			//complete transaction
			return wallet.TakeCurrency(Owallet, a_Rewards.Count());
		}
		else if (a_Rewards.Count() > 0)
		{
			//Get target inventory
			SCR_InventoryStorageManagerComponent TargetInv = SCR_InventoryStorageManagerComponent.Cast(Target.FindComponent(SCR_InventoryStorageManagerComponent));
			SCR_InventoryStorageManagerComponent OwnerInv = SCR_InventoryStorageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
			int Movedamount;
			//itterate through reward and move them to target storage
			for (int i = 0; i < a_Rewards.Count(); i++)
			{
				OwnerInv.TryRemoveItemFromInventory(a_Rewards.Get(i));
				//ADM_Utils.InsertAutoEquipItem(TargetInv, a_Rewards.Get(i));
				//TargetInv.TryInsertItem(a_Rewards.Get(i));
				Movedamount += 1;
			}
			return true;
		}
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	//invoker stuff
	//------------------------------------------------------------------//
	//Owner stuff
	void GetOnOwnerDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveOwnerInvokers();
		//m_eTaskOwner = null;
		//possible to fail task, if so override dis
	}
	void GetOnOwnerRankUp()
	{
		CreateDescritions();
	}
	void AddOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Insert(GetOnOwnerDeath);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Insert(CheckUpdatedAffiliations);
		}
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Insert(GetOnOwnerRankUp);
	}
	void RemoveOwnerInvokers()
	{
		SCR_CharacterDamageManagerComponent dmgmn = SCR_CharacterDamageManagerComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		dmgmn.GetOnDamageStateChanged().Remove(GetOnOwnerDeath);
		SCR_CharacterRankComponent RankCo = SCR_CharacterRankComponent.Cast(m_eTaskOwner.FindComponent(SCR_CharacterRankComponent));
		RankCo.s_OnRankChanged.Remove(GetOnOwnerRankUp);
		if (m_OwnerFaction)
		{
			m_OwnerFaction.OnRelationDropped().Remove(CheckUpdatedAffiliations);
		}
	}
	//------------------------------------------------------------------//
	//Target stuff
	void GetOnTargetDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveTargetInvokers();
		//m_eTaskTarget = null;
		//possible to fail task, if so override dis
	}
	void GetOnTargetRankUp()
	{
		CreateDescritions();
	}
	void AddTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(GetOnTargetDeath);
	}
	void RemoveTargetInvokers()
	{
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_eTaskTarget.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(GetOnTargetDeath);
	}
	
	//------------------------------------------------------------------//
	//Assignee stuff
	//------------------------------------------------------------------------------------------------------------//
	//Assign character to this task
	bool AssignCharacter(IEntity Character)
	{
		// make sure to anassign owner before
		if (m_bOwnerAssigned)
			UnAssignOwner();
		
		//if already assigned return
		if (m_aTaskAssigned == Character)
			return true;
		
		//clear reserves to clear behaviors from AI comming to pickup task
		ClearReserves();
		
		//If player
		if ( GetGame().GetPlayerController().GetControlledEntity() == Character )
		{
			m_aTaskAssigned = Character;
			// update state
			if( e_State == ETaskState.UNASSIGNED )
			{
				e_State = ETaskState.ASSIGNED;
			}
			//invokers
			AddAssigneeInvokers();
			//Spawn task marker for player
			SpawnTaskMarker( Character );
			
			return true;
		}
		//if AI
		else
		{
			m_aTaskAssigned = Character;
			//update state
			if( e_State == ETaskState.UNASSIGNED )
			{
				e_State = ETaskState.ASSIGNED;
			}
			//invokers
			AddAssigneeInvokers();
			
			return true;
		}
		return false;
	}
	//Assign character to this task
	bool AssignOwner()
	{
		m_bOwnerAssigned = true;
		return true;
	}
	void UnAssignOwner()
	{
		m_bOwnerAssigned = false;
	}
	void UnAssignCharacter()
	{
		m_aTaskAssigned = null;
		// if player remove marker
		if (m_TaskMarker)
		{
			m_TaskMarker.Cancel(true);
		}
		//update state
		e_State = ETaskState.UNASSIGNED;
		
		ClearReserves();
	}
	//handle death of assignee
	void GetOnAssigneeDeath(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		RemoveAssigneeInvokers();
		UnAssignCharacter();
		//Decide owner behevior
	}
	void AddAssigneeInvokers()
	{
		if (!m_aTaskAssigned)
			return;
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_aTaskAssigned.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Insert(GetOnAssigneeDeath);
	}
	void RemoveAssigneeInvokers()
	{
		if (!m_aTaskAssigned)
			return;
		ScriptedDamageManagerComponent dmgman = ScriptedDamageManagerComponent.Cast(m_aTaskAssigned.FindComponent(ScriptedDamageManagerComponent));
		dmgman.GetOnDamageStateChanged().Remove(GetOnAssigneeDeath);
	}
	//------------------------------------------------------------------//
	//Check updated affiliation and update task if needed
	void CheckUpdatedAffiliations(SCR_Faction factionA, SCR_Faction factionB = null)
	{
		if (!factionB || !m_eTaskTarget)
		{
			return;
		}
		FactionAffiliationComponent affcomp = FactionAffiliationComponent.Cast(m_eTaskTarget.FindComponent(FactionAffiliationComponent));
		if (affcomp.GetAffiliatedFaction() == factionB)
			CancelTask();
	}
	
	//------------------------------------------------------------------------------------------------------------//
	//Spawn task marker for this task, called when assigning character
	void SpawnTaskMarker(IEntity Assignee)
	{
		//Get task resource
		Resource Marker = Resource.Load( "{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et" );
		//setup spawn params
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		//Getaffiliation of Assignee to add to marker
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast( Assignee.FindComponent( FactionAffiliationComponent ) );
		//get spawn location from target
		m_eTaskTarget.GetWorldTransform(PrefabspawnParams.Transform);
		//spawn marker
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
		
		//set info
		m_TaskMarker.SetTitle( m_sTaskTitle );
		m_TaskMarker.SetDescription( m_sTaskDesc );
		m_TaskMarker.SetTarget( m_eTaskTarget );
		m_TaskMarker.SetTargetFaction( Aff.GetAffiliatedFaction() );
		
		//assign to player
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_aTaskAssigned);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssignee(assignee, 0);
	}
	
	//------------------------------------------------------------------------------------------------------------//
	//Getter of m_iRewardAverageAmount attribute. Used on task samples stored in SP_RequestManagerComponent.
	int GetRewardAverage()
	{
		return m_iRewardAverageAmount;
	};
	//Reward lebal deffines what kind of reward this task will provide. atm only works with EEditableEntityLabel.ITEMTYPE_ITEM and EEditableEntityLabel.ITEMTYPE_WEAPON
	ERequestRewardItemDesctiptor GetRewardLabel()
	{
		return e_RewardLabel;
	}
	//Checks if time limit of task has passed
	bool TimeLimitHasPassed()
	{
		TaskDayTimeInfo CurrentDate = TaskDayTimeInfo.FromTimeOfTheDay();
		return m_TaskTimeInfo.HasPassed(CurrentDate);
	}
	//part of chain tasks work a bit differently. Some checks are skipped and settings are kept from Attribute istead of overriden by sample check init function
	void SetPartOfChain()
	{
		m_bPartOfChain = true;
	}
	//reserves task to not be assignable to other characters
	void SetReserved(IEntity res){UnAssignOwner(); reserved = res;};
	//checks if someone has reserved it
	bool IsReserved()
	{
		if (reserved)
			return true;
		return false;
	}
	//if owner is attempting this task
	bool IsOwnerAssigned()
	{
		return m_bOwnerAssigned;
	}
	//used to clear behavior from AI on task state change
	void ClearReserves()
	{
		if ( IsReserved() )
		{
			//make sure to look for the behavior and complete it on the character that had this task reserved
			AIControlComponent comp = AIControlComponent.Cast(reserved.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			if (agent)
			{
				SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
				SCR_AITaskPickupBehavior act = SCR_AITaskPickupBehavior.Cast(utility.FindActionOfType(SCR_AITaskPickupBehavior));
				//if following this task stop action
				if (act.PickedTask == this)
					act.SetActiveFollowing(false);
			}
		}
		//clear reserved variable
		SetReserved(null);
	};
	//------------------------------------------------------------------------------------------------------------//
	//Deffining structure of Init
	bool Init()
	{
		if (!m_bPartOfChain)
			InheritFromSample();
		
		
		if (!m_eTaskOwner)
		{
			//first look for owner cause targer is usually derived from owner faction/location etc...
			if (!FindOwner(m_eTaskOwner))
			{
				return false;
			}
			//-------------------------------------------------//
			
		}
		//-------------------------------------------------//
		if (!m_eTaskTarget)
		{
			if (!FindTarget(m_eTaskTarget))
			{
				return false;
			}
			
		}
		//-------------------------------------------------//
		//-------------------------------------------------//
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		//function to fill to check ckaracter
		if(!CheckOwner())
		{
			return false;
		}
		if(!CheckTarget())
		{
			return false;
		}
		//-------------------------------------------------//
		SetTimeLimit();
		CreateDescritions();
		AddOwnerInvokers();
		AddTargetInvokers();
		e_State = ETaskState.UNASSIGNED;
		//Set the invoker so the task fails if owner dies
		
		return true;
	};

	
};
class TaskAttribute : BaseContainerCustomTitle
{
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		string enabled;
		bool enable;
		bool assign;
		string assignable;
		source.Get("m_bEnabled", enable);
		if (enable)
			enabled = "ACTIVE";
		else
			enabled = "INACTIVE";
		
		source.Get("m_bAssignable", assign);
		
		if (assign)
			assignable = "Assignable";
		else
			assignable = "UnAssignable";
		
		title = source.GetClassName() + " | " + enabled + " | " + assignable;
		return true;
	}
}
