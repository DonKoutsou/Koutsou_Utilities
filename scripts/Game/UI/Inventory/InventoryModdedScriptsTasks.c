modded class SCR_InventoryMenuUI : UIInfo
{
	string m_sMyDesc;
	string m_sInsulation;
	override void ShowItemInfo( string sName = "", string sDescr = "", float sWeight = 0.0, SCR_InventoryUIInfo uiInfo = null )
	{
		if ( !m_pItemInfo )
		{
			//Widget parent = m_widget.FindAnyWidget( "SoldierInfo" );
			Widget infoWidget = GetGame().GetWorkspace().CreateWidgets(ITEM_INFO, m_widget);
			if ( !infoWidget )
				return;

			infoWidget.AddHandler( new SCR_InventoryItemInfoUI() );
			m_pItemInfo = SCR_InventoryItemInfoUI.Cast( infoWidget.FindHandler( SCR_InventoryItemInfoUI ) );
		}

		if( !m_pItemInfo )
			return;

		Widget w = WidgetManager.GetWidgetUnderCursor();
		if (!w)
		{
			w = m_pFocusedSlotUI.GetButtonWidget();
		}

		m_pItemInfo.Show( 0.6, w, m_bIsUsingGamepad );
		m_pItemInfo.SetName( sName );
		m_pItemInfo.SetDescription( sDescr );
		m_pItemInfo.SetCharName(m_sCharName);
		m_pItemInfo.SetWeight( sWeight );
		m_pItemInfo.SetInsulation(m_sInsulation);
		m_pItemInfo.SetCharName(m_sCharName);
		m_pItemInfo.SetOptDesc(m_sMyDesc);
		if (uiInfo && uiInfo.IsIconVisible())
			m_pItemInfo.SetIcon(uiInfo.GetIconPath(), uiInfo.GetIconColor());

		int iMouseX, iMouseY;

		float x, y;
		w.GetScreenPos(x, y);

		float width, height;
		w.GetScreenSize(width, height);

		float screenSizeX, screenSizeY;
		GetGame().GetWorkspace().GetScreenSize(screenSizeX, screenSizeY);

		float infoWidth, infoHeight;
		m_pItemInfo.GetInfoWidget().GetScreenSize(infoWidth, infoHeight);

		iMouseX = x;
		iMouseY = y + height;
		if (x + infoWidth > screenSizeX)
			iMouseX = screenSizeX - infoWidth - width / 2; // offset info if it would go outside of the screen

		m_pItemInfo.Move( GetGame().GetWorkspace().DPIUnscale( iMouseX ), GetGame().GetWorkspace().DPIUnscale( iMouseY ) );
		
		m_sMyDesc = STRING_EMPTY;
		m_sInsulation = STRING_EMPTY;
		m_sCharName = STRING_EMPTY;
	}
	override protected void SetFocusedSlotEffects()
	{
		if( !m_pFocusedSlotUI )
		{
			if( m_wProgressBarWeightItem )
				m_wProgressBarWeightItem.SetCurrent( 0.0 );
			return;
		}

		//show info about the item
		InventoryItemComponent invItemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		if ( !invItemComp )
			return;
		auto attribs = SCR_ItemAttributeCollection.Cast( invItemComp.GetAttributes() );

		if ( !attribs )
			return;
		UIInfo itemInfo = attribs.GetUIInfo();
		DogTagEntity tag = DogTagEntity.Cast(invItemComp.GetOwner());
		if(tag)
		{
			tag.GetCname(m_sCharName);
			m_sCharName = "TagOwner: " + m_sCharName
		}
		m_sInsulation = attribs.GetInsulation();
		string cname;
		string tname;
		string loc;
		
		SP_UnretrievableComponent uncomp = SP_UnretrievableComponent.Cast(invItemComp.GetOwner().FindComponent(SP_UnretrievableComponent));
		if(uncomp)
		{
			IEntity target;
			SP_PackageComponent package =  SP_PackageComponent.Cast(uncomp);
			if(package)
			{
				package.GetInfo(cname, tname, loc);
				m_sMyDesc = string.Format("Deliver to %1, located on %2", tname, loc);
			}
			SP_BountyComponent bounty =  SP_BountyComponent.Cast(uncomp);
			if(bounty)
			{
				bounty.GetInfo(cname, tname, loc);
				m_sMyDesc = string.Format("Target is %1, located on %2", tname, loc);
			}
		}
		if ( !itemInfo )
			HideItemInfo();
		else
			ShowItemInfo( itemInfo.GetName(), itemInfo.GetDescription(), invItemComp.GetTotalWeight(), SCR_InventoryUIInfo.Cast(itemInfo) );
	
		//show the weight on the progressbar
		//TODO: overlap or add on the end, depending on if the item is already in the storage or is going to be added
		if( m_wProgressBarWeightItem )
		{
			float weight = invItemComp.GetTotalWeight();
			m_wProgressBarWeightItem.SetCurrent( weight );
		};

		NavigationBarUpdate();
	}
}
modded class SCR_InventoryItemInfoUI
{
	private TextWidget						m_wTextInsulation;
	private TextWidget						m_wTextOptDesc;
	
	void SetInsulation( string insulation )
	{
		if( insulation != "" )
		{
			m_wTextInsulation.SetEnabled( true );
			m_wTextInsulation.SetVisible( true );
			m_wTextInsulation.SetText( insulation );
		}
		else
		{
			m_wTextInsulation.SetEnabled( false );
			m_wTextInsulation.SetVisible( false );
		}
	}
	void SetOptDesc(string desc)
	{
		if( desc != "" )
		{
			m_wTextOptDesc.SetEnabled( true );
			m_wTextOptDesc.SetVisible( true );
			m_wTextOptDesc.SetText( desc );
		}
		else
		{
			m_wTextOptDesc.SetEnabled( false );
			m_wTextOptDesc.SetVisible( false );
		}
	}
	override void HandlerAttached( Widget w )
	{
		if( !w )
			return;
		m_infoWidget		= w;
		m_wTextName 		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_name" ) );
		m_wTextDescription 	= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_description" ) );
		m_wTextWeight 		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_weight" ) );
		m_wTextWeightUnit	= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_weightUnit" ) );
		m_wTextInsulation	= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_insulationUnit" ) );
		m_wTextOptDesc		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_OptDesc0" ) );
		m_wTextCharName		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_CharName" ) );
		m_wItemIcon 		= ImageWidget.Cast(w.FindAnyWidget("ItemInfo_icon"));
		Widget wItemInfo	= m_infoWidget.FindAnyWidget( "ItemInfo" );
		if ( !wItemInfo )
			return;
		m_pFrameSlotUI 		= SCR_SlotUIComponent.Cast( wItemInfo.FindHandler( SCR_SlotUIComponent ) );
	}
}