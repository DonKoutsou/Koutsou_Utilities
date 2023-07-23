[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageCommenceTradeAction : DialogueStage
{
	protected ADM_ShopComponent m_Shop;
	override void Perform(IEntity Character, IEntity Player)
	{
		m_Shop = ADM_ShopComponent.Cast(Character.FindComponent(ADM_ShopComponent));
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return;
		
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController) return;
		
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!scrPlayerController) return;
		
		if (Player != scrPlayerController.GetMainEntity()) return;
		
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_ShopMenu); 
		ADM_ShopUI menu = ADM_ShopUI.Cast(menuBase);
		menu.SetShop(m_Shop);
	}
}