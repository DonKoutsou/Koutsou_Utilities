class StoreSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(User);
		if (!char)
			return false;
		SP_StoreAISmartActionComponent storeaction = SP_StoreAISmartActionComponent.Cast(Owner.FindComponent(SP_StoreAISmartActionComponent));
		if (!storeaction)
			return false;
		int needamount;
		array <int> needs = {};
		char.GetAllNeeds(needs);
		foreach (int need : needs)
		{
			
			if (storeaction.shoplist.Contains(need))
			{
				if (need == ERequestRewardItemDesctiptor.AMMO)
				{
					int ammount;
					BaseMagazineComponent mag;
					char.Checkneed(need, ammount, mag);
					if (mag)
					{
						if (storeaction.MagList.Contains(mag.GetMagazineWell()))
						{
							return true;
						}
					}
				}
				else
					return true;
			}
		}
		
		///if (storeaction.TestNeeds(char))
		//{
		//	return true;
		//}
		return false;
	}
}