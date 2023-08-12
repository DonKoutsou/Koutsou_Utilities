modded class SCR_InventorySlotUI
{
		float m_fFoodAmount;
		float m_fFoodAmountMax;
	
		override void SetSlotVisible( bool bVisible )
		{
			m_bVisible = bVisible;
			m_widget.SetEnabled( bVisible );
			m_widget.SetVisible( bVisible );
			
			if( bVisible )
			{
				m_wPreviewImage = RenderTargetWidget.Cast( m_widget.FindAnyWidget( "item" ) );
				ItemPreviewManagerEntity manager = GetGame().GetItemPreviewManager();
				if (manager)
				{
					ItemPreviewWidget renderPreview = ItemPreviewWidget.Cast( m_wPreviewImage );
					IEntity previewEntity = null;
					if (m_pItem)
						previewEntity = m_pItem.GetOwner();
					if (renderPreview)
						manager.SetPreviewItem(renderPreview, previewEntity, null, true);
				}
				
				//if the slot has storage, then show its volume bar
				BaseInventoryStorageComponent pStorageTo = GetStorageComponent();
				if ( pStorageTo )
				{
					m_wVolumeBar = ProgressBarWidget.Cast( m_widget.FindAnyWidget( "VolumeBar" ) );
					m_wVolumeBar.SetVisible( true );
				}
				m_wSelectedEffect = m_widget.FindAnyWidget( "SelectedFrame" );
				m_wMoveEffect = m_widget.FindAnyWidget( "IconMove" );
				m_wDimmerEffect = m_widget.FindAnyWidget( "Dimmer" );
				m_wButton = ButtonWidget.Cast( m_widget.FindAnyWidget( "ItemButton" ) );
				m_wStackNumber = TextWidget.Cast( m_widget.FindAnyWidget( "stackNumber" ) );
				m_wItemLockThrobber = OverlayWidget.Cast(m_widget.FindAnyWidget("itemLockThrobber"));
			
				if ( m_iStackNumber > 1 )
				{
					m_wStackNumber.SetText( m_iStackNumber.ToString() );
					m_wStackNumber.SetVisible( true );
				}
				else
				{
					m_wStackNumber.SetVisible( false );
				}
				
				m_wAmmoCount = ProgressBarWidget.Cast( m_widget.FindAnyWidget( "ammoCount" ) );
				if ( m_wAmmoCount )
				{
					SetAmmoCount();
					UpdateAmmoCount();
					SetFoodCount();
					UpdateFoodAmount();
				}
				
				m_wMagazineNumber = TextWidget.Cast( m_widget.FindAnyWidget( "magazineCount" ) );
				m_wCurrentMagazineAmmoCount = ProgressBarWidget.Cast( m_widget.FindAnyWidget( "currentMagazineAmmoCount" ) );
				if ( m_wMagazineNumber && m_wCurrentMagazineAmmoCount )
				{
					UpdateWeaponAmmoCount();
				}
				
				#ifdef DEBUG_INVENTORY20
					if ( !m_pItem )
					{
						array<string> dbgText = new array<string>();
						this.ToString().Split( "<", dbgText, false );
	 					m_wDbgClassText1 = TextWidget.Cast( m_widget.FindAnyWidget( "dbgTextClass1" ) );
						m_wDbgClassText2 = TextWidget.Cast( m_widget.FindAnyWidget( "dbgTextClass2" ) );
						m_wDbgClassText3 = TextWidget.Cast( m_widget.FindAnyWidget( "dbgTextClass3" ) );
						m_wDbgClassText1.SetText( dbgText[0] );
						m_wDbgClassText1.SetEnabled( true );
						m_wDbgClassText1.SetVisible( true );
						m_wDbgClassText2.SetText( dbgText[1] );
						m_wDbgClassText2.SetEnabled( true );
						m_wDbgClassText2.SetVisible( true );
						m_pItem.ToString().Split( "<", dbgText, false );
						if ( dbgText.Count() > 1 )
						{
							m_wDbgClassText3.SetText( dbgText[1] );
							m_wDbgClassText3.SetEnabled( true );
							m_wDbgClassText3.SetVisible( true );
						}
					}
				#endif
				UpdateVolumeBarValue();
				
			}
			else
			{
				m_wPreviewImage = null;
				m_wSelectedEffect = null;
				m_wMoveEffect = null;
				m_wDimmerEffect = null;
				m_wButton = null;
				m_wVolumeBar = null;
				m_wTextQuickSlot = null;
				m_wTextQuickSlotLarge = null;
				m_wStackNumber = null;
				#ifdef DEBUG_INVENTORY20
					m_wDbgClassText1 = null;
					m_wDbgClassText2 = null;
					m_wDbgClassText3 = null;
				#endif
			}
		}
	
		protected void SetFoodCount()
		{
			if ( !m_pItem )
				return;
			SCR_ConsumableItemComponent pConsumableComponent = SCR_ConsumableItemComponent.Cast( m_pItem.GetOwner().FindComponent( SCR_ConsumableItemComponent ) );
			if ( !pConsumableComponent )
				return;
			if (pConsumableComponent.IsEatable() || pConsumableComponent.IsDrinkable())
			{
				m_fFoodAmount = pConsumableComponent.GetCurrentCapacity();
				m_fFoodAmountMax = pConsumableComponent.GetInitialCapacity();
			}
				
		}
		protected void UpdateFoodAmount()
		{
			if (m_fFoodAmountMax == 0)
				return;
			if( !m_pItem )
				return;
	
			if ( m_fFoodAmount > -1 && m_fFoodAmountMax != 1 )
			{
				m_wAmmoCount.SetMax( m_fFoodAmountMax );
				m_wAmmoCount.SetCurrent( m_fFoodAmount );
				m_wAmmoCount.SetVisible( true );
			}
			else
			{
				m_wAmmoCount.SetVisible( false );
			}
		}
		override void Refresh()
		{
			UpdateVolumeBarValue();
			UpdateStackNumber();
			UpdateAmmoCount();
			UpdateWeaponAmmoCount();
			UpdateFoodAmount();
		}
};