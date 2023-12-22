class SCR_AICreateTask : AITaskScripted
{
	[Attribute("0", UIWidgets.ComboBox, "Wanted task type", "",ParamEnumArray.FromEnum(ETaskType))]
	ETaskType task;
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SP_RequestManagerComponent RequestComp = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		typename taskname;
		switch (task)
		{
			case "KILL":
			{
				taskname = SP_KillTask;
			}
			break;
			case "DELIVER":
			{
				taskname = SP_DeliverTask;
			}
			break;
			case "NAVIGATE":
			{
				taskname = SP_NavigateTask;
			}
			break;
			case "RESCUE":
			{
				taskname = SP_RescueTask;
			}
			break;
			//case "BOUNTY":
			//{
			//	taskname = SP_BountyTask;
			//}
			//break;
			case "RETRIEVE":
			{
				taskname = SP_RetrieveTask;
			}
			break;
		}
		if (!RequestComp.CreateCustomTaskOfType(taskname, owner.GetControlledEntity()))
			return ENodeResult.FAIL;
		return ENodeResult.SUCCESS;
	}
	override bool VisibleInPalette() { return true; }
}
class SCR_AIEvaluatePossibleTags : AITaskScripted
{
	protected const static string TAG_PORT = "Tags";
	protected const static string RADIOUS_PORT = "Radious";
	protected static ref TStringArray s_aVarsOut = {TAG_PORT, RADIOUS_PORT};
	
	override TStringArray GetVariablesOut() { return s_aVarsOut; }

	
	ref array <string> tags;
	int m_iRadious = 20;
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
				m_iRadious = 300;
			}
			else
			{
				//look for work
				tags.Insert("SmartTask");
				m_iRadious = 300;
			}
		}
		else
		{
			tags.Insert("LightFire");
			tags.Insert("SwitchLight");
			tags.Insert("SwitchRadio");
			tags.Insert("DeadBody");
			m_iRadious = 30;
		}
		
		SetVariableOut(RADIOUS_PORT, m_iRadious);
		SetVariableOut(TAG_PORT, tags);
		return ENodeResult.SUCCESS;
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
	override bool VisibleInPalette() { return true; }
}
class SCR_AIPickupTask : AITaskScripted
{
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		array <IEntity> Chars = {};
		
		return ENodeResult.SUCCESS;
	}
	
	override bool VisibleInPalette() { return true; }
}
class SCR_AIGetCurrency : AITaskScripted
{
	protected const static string CURRENCY_PORT = "Currency";
	
	
	protected static ref TStringArray s_aVarsOut = {CURRENCY_PORT};
	
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(owner.GetControlledEntity());
		if (!Char)
			return ENodeResult.FAIL;
		WalletEntity wallet = Char.GetWallet();
		if (!wallet)
			return ENodeResult.FAIL;
		int Currency = wallet.GetCurrencyAmmount();
		SetVariableOut(CURRENCY_PORT, Currency);
		return ENodeResult.SUCCESS;
	}
	
	override bool VisibleInPalette() { return true; }
}
class HasTaskSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if (!requestman.CharHasTask(Owner))
		{
			return false;
		}
		array <ref SP_Task> tasks = {};
		requestman.GetCharTasks(Owner, tasks);
		foreach (SP_Task task : tasks)
		{
			if (task.GetTarget() == User)
				continue;
			if (task.IsReserved())
					continue;
			if (task.GetState() == ETaskState.ASSIGNED)
					continue;
			if (task.GetTimeLimit() < 3 && task.GetTimeLimit() != -1)
					continue;
			return true;
		}
		return false;
	}
}