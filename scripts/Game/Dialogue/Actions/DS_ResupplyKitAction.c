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
		for (int i = 0; i < foundkits.Count(); i++)
		{
			SCR_ResourceComponent rscrs = SCR_ResourceComponent.Cast(foundkits[i].FindComponent(SCR_ResourceComponent));
			if (rscrs)
			{
				rscrs.GetContainer(EResourceType.SUPPLIES).MaxOutResourceValue();
			}
		}
	};
};