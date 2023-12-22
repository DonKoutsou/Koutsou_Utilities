[BaseContainerProps(configRoot:true), DialogueStageActionTitleAttribute()]
class DS_DialogueStageResupplyKitAction : DS_BaseDialogueStageAction
{
	[Attribute()]
	ResourceName m_KitPrefab;
	override void Perform(IEntity Character, IEntity Player)
	{

		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		PrefabResource_Predicate Pred = new PrefabResource_Predicate(m_KitPrefab);
		array <IEntity> foundkits = new array <IEntity>();
		inv.FindItems(foundkits, Pred);
		int ammounttifill;
		SCR_ChimeraCharacter plchar = SCR_ChimeraCharacter.Cast(Player);
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(Character);
		WalletEntity walletpl = plchar.GetWallet();
		WalletEntity wallet = char.GetWallet();
		int monez = wallet.GetCurrencyAmmount();
		array<IEntity> MoneysToGive = {};
		for (int i = 0; i < foundkits.Count(); i++)
		{
			SCR_ResourceComponent rscrs = SCR_ResourceComponent.Cast(foundkits[i].FindComponent(SCR_ResourceComponent));
			if (rscrs)
			{
				SCR_ResourceContainer container = rscrs.GetContainer(EResourceType.SUPPLIES);
				if (container.GetMaxResourceValue() > container.GetResourceValue())
				{
					ammounttifill = ammounttifill + container.GetMaxResourceValue() - container.GetResourceValue();
					if (ammounttifill > monez)
					{
						ammounttifill = monez;
						rscrs.GetContainer(EResourceType.SUPPLIES).SetResourceValue(container.GetResourceValue() + ammounttifill);
						wallet.TakeCurrency(walletpl, ammounttifill);
						break;
					}
					rscrs.GetContainer(EResourceType.SUPPLIES).MaxOutResourceValue();
					wallet.TakeCurrency(walletpl, ammounttifill);
				}
			}
		
			return;
		};
	}
};