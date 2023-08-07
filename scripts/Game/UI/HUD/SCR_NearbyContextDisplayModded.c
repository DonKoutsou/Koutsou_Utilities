//------------------------------------------------------------------------------------------------
modded class SCR_NearbyContextDisplay
{
	override void DisplayUpdate(IEntity owner, float timeSlice)
	{
		// m_pInteractionHandlerComponent.SetNearbyCollectionEnabled(true);
		if (!m_pInteractionHandlerComponent.GetNearbyCollectionEnabled())
		{
			// Were we drawing something?
			// If yes, then disable all
			if (m_iVisibleWidgets > 0)
				SetVisibleWidgets(0);
		
			return;
		}
		
		// Fetch nearby contexts
		array<UserActionContext> contexts = {};
		int count = m_pInteractionHandlerComponent.GetNearbyAvailableContextList(contexts);
		
		// Prepare widgets
		int actualCount = 0;
		
		// Get required data
		BaseWorld world = owner.GetWorld();
		int cameraIndex = world.GetCurrentCameraId();
		vector mat[4];
		world.GetCamera(cameraIndex, mat);
		vector referencePos = mat[3];
		
		// Iterate through individual contexts,
		// validate that they are visible,
		// and update their widget representation
		const float threshold = 0.25;
		const float fovBase = 100; // whatever fits
		
		// Field of view and screen resolution is necessary to compute proper position and size of widgets
		float zoom = 1; // world.GetCameraVerticalFOV(cameraIndex) - missing crucial getter
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (cameraManager)
		{
			CameraBase camera = cameraManager.CurrentCamera();
			if (camera)
				zoom = fovBase / Math.Max(camera.GetVerticalFOV(), 1);
		}
		float distanceLimit =  m_fWidgetMaximumRange * zoom;
		float distanceLimitSq = distanceLimit * distanceLimit;
		// Screen resolution is necessary to know how far away the widget is from screen center
		// or from cursor, if we ever allow player to use mouse cursor or eye tracking software to select the actions.
		float resX; float resY; 
		GetGame().GetWorkspace().GetScreenSize(resX, resY);
		
		UserActionContext currentContext = m_pInteractionHandlerComponent.GetCurrentContext();
		foreach (UserActionContext ctx : contexts)
		{
			// Do not draw currently select one
			if (currentContext == ctx)
				continue;
			
			// We already have too much
			if (actualCount >= m_iPrecachedWidgetCount)
				break;
			
			vector position = ctx.GetOrigin();
			float distanceSq = vector.DistanceSq(position, referencePos);
			
			// Just ignore actions out of reach, we will fade them out anyway
			if (distanceSq < distanceLimitSq && IsInLineOfSight(position, mat, threshold))
			{
				Widget widget =  m_aWidgets[actualCount];
				if (widget)
				{
					FrameWidget root = FrameWidget.Cast(widget);
					SetWidgetWorldPosition(world, cameraIndex, widget, position);
					TextWidget Tw = TextWidget.Cast(root.FindAnyWidget("BlipName"));
					if (Tw)
					{
						float distance = Math.Sqrt(distanceSq);
						bool visible = ctx.IsInVisibilityAngle(referencePos);
						array<BaseUserAction> outActions = new array<BaseUserAction>();
						ctx.GetActionsList(outActions);
						LocalizedString text;
						if (outActions.IsEmpty())
							continue;
						
						IEntity disowner = outActions[0].GetOwner();
						if (!disowner)
							continue;
						if (ChimeraCharacter.Cast(disowner) || Vehicle.Cast(disowner))
							continue;
						InventoryItemComponent inv = InventoryItemComponent.Cast(disowner.FindComponent(InventoryItemComponent));
						if (!inv)
						{
							continue;
						}
						else
							text = inv.GetUIInfo().GetName();
						if (text)
							Tw.SetText(text);
						else 
							Tw.SetOpacity(0);
						//SetWidgetScale(Tw, distance, zoom);
						//SetWidgetAlpha(Tw, distance, distanceLimit, resX, resY);
						//SetWidgetColor(Tw, distance, visible);
					}
					// TODO@AS:
					// First child is an image.
					// We need more robust solution if not.
					Widget child = root.FindAnyWidget("BlipElement");
					if (child)
					{
						// distance^2 from context to camera origin
						float distance = Math.Sqrt(distanceSq);
						bool visible = ctx.IsInVisibilityAngle(referencePos);
						SetWidgetScale(child, distance, zoom);
						SetWidgetAlpha(child, distance, distanceLimit, resX, resY);
						SetWidgetColor(child, distance, visible);
					}
				
					
				}
				actualCount++;
			}
		}
		
		// Enable required amount of widgets
		SetVisibleWidgets(actualCount);
	}
	override protected Widget FindIconWidget(Widget layout)
	{
		return layout.GetChildren().GetChildren();
	}
};