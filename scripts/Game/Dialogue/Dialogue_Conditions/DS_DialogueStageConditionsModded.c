[BaseContainerProps(configRoot:true)]
class DS_DialogueStageResupplyKitActionCondition : DS_BaseDialogueStageActionCondition
{
	[Attribute()]
	ResourceName m_KitPrefab;

	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		PrefabResource_Predicate Pred = new PrefabResource_Predicate(m_KitPrefab);
		array <IEntity> foundkits = new array <IEntity>();
		inv.FindItems(foundkits, Pred);
		bool haskittofill;
		int ammounttifill;
		for (int i = 0; i < foundkits.Count(); i++)
		{
			SCR_ResourceComponent rscrs = SCR_ResourceComponent.Cast(foundkits[i].FindComponent(SCR_ResourceComponent));
			if (rscrs)
			{
				SCR_ResourceContainer container = rscrs.GetContainer(EResourceType.SUPPLIES);
				if (container.GetMaxResourceValue() > container.GetResourceValue())
				{
					haskittofill = true;
					ammounttifill = ammounttifill + container.GetMaxResourceValue() - container.GetResourceValue();
				}
			}
		}
		if (!haskittofill)
			return false;
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(Player);
		WalletEntity wallet = char.GetWallet();
		if (!wallet)
			return false;
		int cutam = wallet.GetCurrencyAmmount();
		if (cutam < ammounttifill)
			return false;
		
		return true;
	}
}