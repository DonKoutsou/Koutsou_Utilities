//action to be used by AI to buy stuff, based on their needs wich should be configured in the smart action component of the store
class SP_BuyStuff : ScriptedUserAction
{
	SP_StoreAISmartActionComponent StoreSmartAction;
	ADM_ShopComponent shop;
	ref array <ref ADM_ShopMerchandise> Merchendise
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		//Get chimera
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast( pUserEntity );
		
		if ( ! Char )
			return;
		
		int ammount;
		BaseMagazineComponent mag;
		array <int> needs = {};
		
		//Gather needs of character
		Char.GetAllNeeds(needs);
		
		//final need to select and fulfull
		ERequestRewardItemDesctiptor needtofulfill;
		array <int> fulfillableneeds = {};
		//Check if any of needs can be fulfilled and get its info
		foreach (int need : needs)
		{
			if (StoreSmartAction.shoplist.Contains(need))
			{
				if (Char.Checkneed(need ,ammount, mag))
				{
					fulfillableneeds.Insert( need);
					break;
				}
			}
		}
		if ( fulfillableneeds.IsEmpty())
			return;
		
		needtofulfill = fulfillableneeds.GetRandomElement();
		
		if (!needtofulfill)
			return;
		
		//find request in catalog
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType( EEntityCatalogType.REQUEST );
		
		array < SCR_EntityCatalogEntry > Mylist = {};
		//Get items that match my need
		RequestCatalog.GetRequestItems(needtofulfill, mag, Mylist);
		//Get worth of any of those items
		int price = RequestCatalog.GetWorthOfItem(Mylist.GetRandomElement().GetPrefab());
		// array to fill with stuff character can afford
		array <ref ADM_ShopMerchandise> CanBuyMerchendise = {};
		//itterate through list and check wich of the items can be purchased
		foreach (SCR_EntityCatalogEntry entry : Mylist)
		{
			ResourceName prefab = entry.GetPrefab();
			foreach (ADM_ShopMerchandise merch : Merchendise)
			{
				if (prefab == merch.GetMerchandise().GetPrefab())
				{
					if (shop.CanPurchase(pUserEntity, merch, ammount))
					{
						CanBuyMerchendise.Insert(merch);
					}
				}
			}
		}
		//none return
		if (CanBuyMerchendise.IsEmpty())
			return;
		//make sure to remove headgear when buying helmet so that helmet can be equipped instantly
		if (needtofulfill == ERequestRewardItemDesctiptor.HELMET)
		{
			RemoveHeadgear(pUserEntity);
		}
		// make purchase
		shop.AskAIPurchase(pUserEntity, CanBuyMerchendise.GetRandomElement(), ammount);
		return;
		
	}
	//function used to check if character has task for this need and if yes cancel it. Used when a need is fulfilled
	void CompleteTasks(IEntity Owner, ERequestRewardItemDesctiptor need)
	{
		//task array
		array <ref SP_Task> tasks = {};
		
		//get tasks of character
		SP_RequestManagerComponent.GetCharTasksOfSameType( Owner , tasks , SP_RetrieveTask );
		
		//if no tasks on character return
		if ( tasks.IsEmpty( ) )
			return;
		
		//itterate through them and if need matches, cancel it and return
		foreach ( SP_Task task : tasks )
		{
			SP_RetrieveTask rtask = SP_RetrieveTask.Cast( task );
			if ( ! rtask )
				continue;
			if ( rtask.m_requestitemdescriptor == need )
			{
				task.CancelTask();
				return;
			}
		}
	}
	//function used to remove headgear when buying a helmet
	void RemoveHeadgear(IEntity char)
	{
		//Get inventory of character
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast( char.FindComponent( SCR_CharacterInventoryStorageComponent ) );
		
		//Get headgear cloth
		IEntity Hat = loadoutStorage.GetClothFromArea( LoadoutHeadCoverArea );
		
		//return if none
		if (!Hat)
			return;
		
		// get inventory manager
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast( char.FindComponent( SCR_InventoryStorageManagerComponent ) );
		
		//Get inventory component of headgear
		InventoryItemComponent pInvComp = InventoryItemComponent.Cast( Hat.FindComponent( InventoryItemComponent ) );
		
		// get storage its in
		InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
		
		//try to remove it
		inv.TryRemoveItemFromStorage(Hat, parentSlot.GetStorage());
	}
	//Function used to check a price of a need to decide if able to fulfill it
	void CheckNeedPrice(ERequestRewardItemDesctiptor need, BaseMagazineComponent mag, out int price)
	{
		//Get request catalog
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType( EEntityCatalogType.REQUEST );
		
		//Get items matching my need
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(need, mag, Mylist);
		
		if (Mylist.IsEmpty())
			return;
		
		//Return price of one of them //Need to im
		price = RequestCatalog.GetWorthOfItem(Mylist.GetRandomElement().GetPrefab());
	}
	
	override event void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		//Get smart action component from store
		StoreSmartAction = SP_StoreAISmartActionComponent.Cast(	pOwnerEntity.FindComponent( SP_StoreAISmartActionComponent ) );
		
		if ( ! StoreSmartAction )
			return;
		
		//find shop component
		shop = ADM_ShopComponent.Cast( pOwnerEntity.FindComponent( ADM_ShopComponent ) );
		
		if ( ! shop )
			return;
		
		//Get merchendise
		if (!Merchendise)
			Merchendise = {};
		
		Merchendise = shop.GetMerchandise();
	};
	//------------------------------------------------------------------//
	override bool CanBeShownScript(IEntity user)
	{
		return false;
	}

	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//----------------------------------------------------------------------------------
	override bool CanBroadcastScript() 
	{ 
		return false; 
	};
}
