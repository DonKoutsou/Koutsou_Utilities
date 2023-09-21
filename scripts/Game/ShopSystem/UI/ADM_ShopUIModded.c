modded class ADM_ShopUIBarterItemIcon
{
	override void OnHoverDetected()
	{
		/*"{553FDBD4C8719782}UI/Layouts/ShopSystem/BarterItemTooltipModded.layout"
		Widget w = SCR_TooltipManagerEntity.CreateTooltip(, m_wRoot);
		TextWidget wText = TextWidget.Cast(w.FindAnyWidget("Text"));
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(w.FindWidget("Overlay.VerticalLayout0.SizeLayout0.PreviewImage"));
		
		if (m_PaymentMethod) {
			int quantity = ADM_ShopUI.FindShopUIItem(m_wRoot).GetQuantity();
			string displayString = m_PaymentMethod.GetDisplayString(quantity);
			wText.SetText(displayString);
			
			m_iPreviewEntity = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_PaymentMethod.GetDisplayEntity()), null, null);
			if (wRenderTarget && m_PreviewManager && m_iPreviewEntity) m_PreviewManager.SetPreviewItem(wRenderTarget, m_iPreviewEntity, null, true);
		}*/
	}
}