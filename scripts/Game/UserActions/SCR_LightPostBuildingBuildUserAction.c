//------------------------------------------------------------------------------------------------
class SCR_LightPostBuildingBuildUserAction : SCR_ScriptedUserAction
{
	protected IEntity m_User;
	protected SCR_GadgetManagerComponent m_GadgetManager;
	protected LightPost m_Post;

	protected override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_Post = LightPost.Cast(pOwnerEntity);
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		EntitySlotInfo slot = SlotManagerComponent.Cast(pOwnerEntity.FindComponent(SlotManagerComponent)).GetSlotByName("Lantern");
		slot.GetAttachedEntity().SetFlags(EntityFlags.VISIBLE);
		pOwnerEntity.SetFlags(EntityFlags.VISIBLE);
		SCR_Global.SetMaterial(pOwnerEntity, "{5DFBCBD72FD4CEED}Assets/Props/Civilian/Birdhouse_01/Data/Birdhouse_01.emat", false);
		slot.GetDetachedEntityInvoker().Insert(RemoveEnt);
		slot.DetachEntity(false);
		slot.GetDetachedEntityInvoker().Remove(RemoveEnt);
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = pOwnerEntity.GetOrigin();
		Resource res = Resource.Load("{50E6A3845C9F20FD}prefabs/Props/Military/Camps/LanternMilitary_US_01.et");
		IEntity Lantern = GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);
		slot.AttachEntity(Lantern);
		Lantern.SetFlags(EntityFlags.VISIBLE);
		SCR_BaseInteractiveLightComponent light = SCR_BaseInteractiveLightComponent.Cast(Lantern.FindComponent(SCR_BaseInteractiveLightComponent));
		if (light)
			light.ToggleLight(true);
		m_Post.SetBuilt(true);
	}
	void RemoveEnt(IEntity Entity)
	{
		if (Entity.GetParent())
		{
			Entity.GetParent().RemoveChild(Entity);
		}
		delete Entity;
	}
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		
		if (!ShouldPerformPerFrame())
			return;

		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;

		CharacterControllerComponent charController = character.GetCharacterController();
		if (charController)
		{
			CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
			int itemActionId = pAnimationComponent.BindCommand("CMD_Item_Action");
			charController.TryUseItemOverrideParams(GetBuildingTool(pUserEntity), false, true, itemActionId, 1, 0, int.MAX, 0, 0, false, null);
		}
		GetGame().GetCallqueue().CallLater(CancelPlayerAnimation, 10000, false, pUserEntity);
		GetGame().GetCallqueue().CallLater(PerformAction, 10000, false, GetOwner(), pUserEntity);
		
		super.OnActionStart(pUserEntity);
	}

	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;

		CharacterControllerComponent charController = character.GetCharacterController();
		if (charController)
		{
			CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
			int itemActionId = pAnimationComponent.BindCommand("CMD_Item_Action");
			CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(pAnimationComponent.GetCommandHandler());
			if (cmdHandler)
				cmdHandler.FinishItemUse();
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnConfirmed(IEntity pUserEntity)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;

		CharacterControllerComponent charController = character.GetCharacterController();
		if (charController)
		{
			CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
			CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(pAnimationComponent.GetCommandHandler());
			cmdHandler.FinishItemUse();
		}
	}

	//------------------------------------------------------------------------------------------------
	//~ If continues action it will only execute everytime the duration is done
	override void PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		if (!LoopActionUpdate(timeSlice))
			return; 
		
		PerformAction(pOwnerEntity, pUserEntity);
	}

	//------------------------------------------------------------------------------------------------
	// The user action is shown when the preview is visible - means player has a building tool ready (but also when he is running around with building tool hiden)
	override bool CanBeShownScript(IEntity user)
	{
		m_User = user;
		return !m_Post.IsBuilt();
	}
	
	//------------------------------------------------------------------------------------------------
	// The user action can be performed, when player has a building tool in hands.
	override bool CanBePerformedScript(IEntity user)
	{	
		m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(user);
		if (!m_GadgetManager)
		{
			m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(user);
			
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			if (playerController)
				playerController.m_OnControlledEntityChanged.Insert(SetNewGadgetManager);
			
			return false;
		}
		
		SCR_GadgetComponent gadgetComponent = m_GadgetManager.GetHeldGadgetComponent();
		if (!gadgetComponent)
			return false;
		
		
		return (gadgetComponent.GetMode() == EGadgetMode.IN_HAND);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stops player animation
	protected void CancelPlayerAnimation(IEntity pUserEntity)
	{
		if (!pUserEntity)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;

		CharacterControllerComponent charController = character.GetCharacterController();
		if (!charController)
			return;

		CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
		if (!pAnimationComponent)
			return;

		CharacterCommandHandlerComponent handlerComponent = pAnimationComponent.GetCommandHandler();
		if (!handlerComponent)
			return;

		handlerComponent.FinishItemUse();
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_User)
			return false;

		return super.GetActionNameScript(outName);
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets a new gadget manager. Controlled by an event when the controlled entity has changed.
	void SetNewGadgetManager(IEntity from, IEntity to)
	{
		m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(to);
	}

	//------------------------------------------------------------------------------------------------
	//! Get building tool entity
	IEntity GetBuildingTool(notnull IEntity ent)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(ent);
		if (!gadgetManager)
			return null;

		return gadgetManager.GetHeldGadget();
	}

	//------------------------------------------------------------------------------------------------
	// Gets the "building tool value" of building tool. It means how many points are added by one usage of the building tool
	int GetBuildingToolValue(notnull IEntity ent)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(ent);
		if (!gadgetManager)
			return 0;

		SCR_CampaignBuildingGadgetToolComponent gadgetComponent = SCR_CampaignBuildingGadgetToolComponent.Cast(gadgetManager.GetHeldGadgetComponent());
		if (!gadgetComponent)
			return 0;

		return gadgetComponent.GetToolConstructionValue();
	}
	
	//------------------------------------------------------------------------------------------------
	void ProcesXPreward()
	{		
		BaseGameMode gameMode = GetGame().GetGameMode();
		if (!gameMode)
			return;
		
		SCR_CampaignBuildingManagerComponent campaignBuildingManagerComponent = SCR_CampaignBuildingManagerComponent.Cast(gameMode.FindComponent(SCR_CampaignBuildingManagerComponent));
		campaignBuildingManagerComponent.ProcesXPreward();
	}

}
