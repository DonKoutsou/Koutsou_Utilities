//added new function to be used by AI when purchasing, skipps check for player manager
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