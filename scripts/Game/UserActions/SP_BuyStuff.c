class SP_BuyStuff : ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!Char)
			return;
		SP_StoreAISmartActionComponent StoreSmartAction = SP_StoreAISmartActionComponent.Cast(pOwnerEntity.FindComponent(SP_StoreAISmartActionComponent));
		if (!StoreSmartAction)
			return;
		
		ADM_ShopComponent shop = ADM_ShopComponent.Cast(pOwnerEntity.FindComponent(ADM_ShopComponent));
		if (!shop)
			return;
		array <ref ADM_ShopMerchandise> Merchendise = shop.GetMerchandise();
		
		int ammount;
		ERequestRewardItemDesctiptor need = Char.GetNeed(ammount);
		
		//find request in catalog
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(need, Mylist);
		int price = RequestCatalog.GetWorthOfItem(Mylist.GetRandomElement().GetPrefab());
		
		foreach (SCR_EntityCatalogEntry entry : Mylist)
		{
			ResourceName prefab = entry.GetPrefab();
			foreach (ADM_ShopMerchandise merch : Merchendise)
			{
				if (prefab == merch.GetMerchandise().GetPrefab())
				{
					if (shop.CanPurchase(pUserEntity, merch, ammount))
					{
						if (shop.AskAIPurchase(pUserEntity, merch, ammount))
							return;
					}
				}
			}
		}
	}
	void CheckNeedPrice(ERequestRewardItemDesctiptor need, out int price)
	{
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		array<SCR_EntityCatalogEntry> Mylist = {};
		RequestCatalog.GetRequestItems(need, Mylist);
		price = RequestCatalog.GetWorthOfItem(Mylist.GetRandomElement().GetPrefab());
	}
	override event void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
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
modded class ADM_ShopBaseComponent
{
	bool AskAIPurchase(IEntity player, ADM_ShopMerchandise merchandise, int quantity)
	{
		if (!player)
			return false;
		
		bool canPay = CanPurchase(player, merchandise, quantity);
		if (!canPay) 
		{
			return false;
		}
		
		bool canDeliver = merchandise.GetMerchandise().CanDeliver(player, this, quantity);
		if (!canDeliver) 
		{
			return false;
		}
		
		if (!merchandise.GetMerchandise().CanPurchaseMultiple() && quantity > 1)
			quantity = 1;
		
		array<ADM_PaymentMethodBase> collectedPaymentMethods = {};
		array<bool> didCollectPayments = {};
		
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPayment();
		foreach (ADM_PaymentMethodBase paymentMethod : requiredPayment) 
		{
			bool didCollectPayment = paymentMethod.CollectPayment(player, quantity);
			didCollectPayments.Insert(didCollectPayment);
			
			if (didCollectPayment) 
				collectedPaymentMethods.Insert(paymentMethod);
		}
		
		if (didCollectPayments.Contains(false))
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player, quantity);
				if (!returnedPayment) 
					PrintFormat("Error returning payment! %s", paymentMethod.Type().ToString());
			}
			
			return false;
		}
		
		bool deliver = merchandise.GetMerchandise().Deliver(player, this, quantity);
		if (!deliver) 
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player, quantity);
				if (!returnedPayment) 
					PrintFormat("Error returning payment! %1", paymentMethod.Type().ToString());
			}
			
			return false;
		}
		return true;	
	}
}