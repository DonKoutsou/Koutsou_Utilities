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
		ERequestRewardItemDesctiptor desc = char.GetNeed(ammount, Mag);
		if (desc)
		{
			int money = char.GetWallet().GetCurrencyAmmount();
			int worth;
			CheckNeedPrice(desc, Mag, worth);
			if (worth * ammount < money)
			{
				//look for store
				tags.Insert("StorePost");
				Radious = 300;
			}
			else
			{
				//look for work
				//tags.Insert("SmartTask");
				//Radious = 300;
				desc = null;
			}
		}
		if (!desc)
		{
			tags.Insert("LightFire");
			tags.Insert("SwitchLight");
			tags.Insert("SwitchRadio");
			tags.Insert("DeadBody");
			Radious = 30;
		}
		GetVariableIn(POISSITION_PORT, Origin);
		if (tags.IsEmpty() && m_aTags)
			tags.Copy(m_aTags);
		if (!Radious)
			Radious = m_fRadius;
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
		if (!Origin || !Radious || tags.IsEmpty())
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		Owner = owner.GetControlledEntity();
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