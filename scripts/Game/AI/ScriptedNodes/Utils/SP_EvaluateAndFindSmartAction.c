class SCR_AIEvaluateAndFindSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	protected bool m_bForceNodeSuccess;
	
	[Attribute()]
	float m_fRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	protected static const string OUT_TAG_PORT = "OutTag";
	protected static const string OUT_CROUCH_BOOL = "CROUCH_BOOL";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radious;
	bool CloseSpawn;
	IEntity Owner;
	ref array <string> tags = new ref array <string>();
	ref array <Managed> correctsmartacts;
	ref array <string> outtags = new ref array <string>();
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
	
		tags = {};
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(owner.GetControlledEntity());
		int ammount;
		BaseMagazineComponent Mag;
		array <ERequestRewardItemDesctiptor> Needs = {};
		ERequestRewardItemDesctiptor desc;
		char.GetAllNeeds(Needs);
		
		
		GetVariableIn(POISSITION_PORT, Origin);
		//if (tags.IsEmpty() && m_aTags)
			//tags.Copy(m_aTags);
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
		if (!Origin)
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		Owner = owner.GetControlledEntity();
		
		if (!Needs.IsEmpty())
		{
			foreach (ERequestRewardItemDesctiptor need : Needs)
			{
				desc = need;
				array <ERequestRewardItemDesctiptor>needstocheck = {};
				needstocheck.Insert(need);
				if (!char.Checkneed(needstocheck, ammount, Mag))
				{
					continue;
				}
				int money = char.GetWallet().GetCurrencyAmmount();
				int worth;
				CheckNeedPrice(desc, Mag, worth);
				if (worth * ammount < money)
				{
					//look for store
					tags.Insert("StorePost");
					Radious = 300;
					GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
					if (correctsmartacts.IsEmpty())
					{
						if (!HasTaskForNeed(char, desc))
							CreateTask(char, desc, ammount, Mag);
					}
					tags.Clear();
				}
				else
				{
						
					//look for work
					//tags.Insert("SmartTask");
					//Radious = 300;
					desc = null;
				}
			}
		}
		Radious = 30;
		tags.Insert("LightFire");
		tags.Insert("SwitchLight");
		tags.Insert("SwitchRadio");
		tags.Insert("DeadBody");
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radious, QueryEntitiesForSmartAction);
		
		if (correctsmartacts.IsEmpty())
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		float dist;
		foreach (Managed SmartA : correctsmartacts)
		{
			SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast(SmartA);
			
			if (!dist)
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
			else if (dist > vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin()))
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
		}
		
		if (OutSmartAction)
		{
			SetVariableOut(OUT_CROUCH_BOOL, OutSmartAction.ShouldCrouchWhenPerforming);
			SetVariableOut(SMARTACTION_PORT, OutSmartAction);
			outtags.Clear();
			OutSmartAction.GetTags(outtags);
			SetVariableOut(OUT_TAG_PORT, outtags);
			correctsmartacts.Clear();
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		if(m_bForceNodeSuccess)
			return ENodeResult.SUCCESS;
		return ENodeResult.FAIL;
	}
	bool HasTaskForNeed(SCR_ChimeraCharacter char, ERequestRewardItemDesctiptor Need)
	{
		SP_RequestManagerComponent Requestman = SP_RequestManagerComponent.GetInstance();
		array <ref SP_Task> tasks = {};
		 Requestman.GetCharTasksOfSameType(char, tasks, SP_RetrieveTask);
		if (tasks.IsEmpty())
			return false;
		foreach (SP_Task task : tasks)
		{
			SP_RetrieveTask rtask = SP_RetrieveTask.Cast(task);
			if (!rtask)
				continue;
			if (rtask.m_requestitemdescriptor == Need)
			{
				return true;
			}
		}
		return false;
	}
	bool CreateTask(SCR_ChimeraCharacter char, ERequestRewardItemDesctiptor Need, int ammount, BaseMagazineComponent Mag = null)
	{
		SP_RequestManagerComponent Requestman = SP_RequestManagerComponent.GetInstance();
		return Requestman.CreateCustomRetrieveTask(char, Need, ammount, Mag);
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		if (!correctsmartacts)
		{
			correctsmartacts = {};
		}
		e.FindComponents(SCR_AISmartActionComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		else
		{
			foreach (Managed Smart : smartacts)
			{
				array <string> demtags2 = new array <string>();
				SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				
				smatcomp.GetTags(demtags2);
				foreach (string tg : demtags2)
				{
					if (tags.Contains(tg))
					{
						if (smatcomp.NeedsTest())
						{
							if (smatcomp.RunTest(Owner))
								correctsmartacts.Insert(Smart);
						}
						else
							correctsmartacts.Insert(Smart);
					}
						
				}
			}
		}
		return true;
	}
	void CheckNeedPrice(ERequestRewardItemDesctiptor need, BaseMagazineComponent mag, out int price)
	{
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(need, mag, Mylist);
		if (Mylist.IsEmpty())
			return;
		price = RequestCatalog.GetWorthOfItem(Mylist.GetRandomElement().GetPrefab());
	}
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT, OUT_TAG_PORT ,OUT_CROUCH_BOOL};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}