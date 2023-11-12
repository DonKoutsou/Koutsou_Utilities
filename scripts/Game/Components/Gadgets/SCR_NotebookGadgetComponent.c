[EntityEditorProps(category: "GameScripted/Gadgets", description: "Notebook gadget", color: "0 0 255 255")]
class SCR_NotebookGadgetComponentClass: SCR_GadgetComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Map gadget component
class SCR_NotebookGadgetComponent : SCR_GadgetComponent
{
	[Attribute("0.3", UIWidgets.EditBox, desc: "seconds, delay before notebook gets activated giving time for the animation to be visible", params: "1 1000", category: "Map")]
	protected float m_fActivationDelay;
				
	protected bool m_bIsNotebookOpen;
	protected bool m_bIsFirstTimeOpened = true;		// whether the notebook has bene opened since put into a lot
	
	//------------------------------------------------------------------------------------------------
	//! Switch between map view
	//! \param state is desired state: true = open, false = close
	void SetNotebookMode(bool state)
	{		
		if (!m_CharacterOwner || !GetGame().GetGameMode())
			return;
				
		// no delay/fade for forced cancel
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(m_CharacterOwner.FindComponent(SCR_CharacterControllerComponent));
		
		float delay; 
		if (!controller.IsDead()) 
			delay = m_fActivationDelay * 1000;

				
		GetGame().GetCallqueue().Remove(ToggleNotebookGadget);
		
		if (state)
		{
			GetGame().GetCallqueue().CallLater(ToggleNotebookGadget, delay, false, true);
		}
		else		
		{
			GetGame().GetCallqueue().CallLater(ToggleNotebookGadget, delay, false, false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Open/close map
	//! \param state is desired state: true = open, false = close
	protected void ToggleNotebookGadget(bool state)
	{			
		if (state)
		{
			MenuManager menuManager = g_Game.GetMenuManager();
			MenuBase myMenu = menuManager.OpenMenu(ChimeraMenuPreset.NotebookMenu);
			GetGame().GetInputManager().ActivateContext("NotebookContext");
			SP_NotebookUI NotebookUI = SP_NotebookUI.Cast(myMenu);
			NotebookUI.Init(GetOwner(), m_CharacterOwner);
			m_bIsNotebookOpen = true;
		}
		else
		{			
			MenuManager menuManager = g_Game.GetMenuManager();
			menuManager.CloseMenuByPreset(ChimeraMenuPreset.NotebookMenu);
			m_bIsNotebookOpen = false;
		}		
	}
				
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	protected void OnNotebookOpen(MapConfiguration config)
	{
		
		// first open
		if (!m_bIsFirstTimeOpened)
			return;
		
		m_bIsFirstTimeOpened = false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	protected void OnNotebookClose(MapConfiguration config)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void ActivateGadgetFlag()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		super.ModeSwitch(mode, charOwner);
			
		if (mode == EGadgetMode.ON_GROUND)
			m_bIsFirstTimeOpened = true;
				
		// not current player	
		IEntity controlledEnt = SCR_PlayerController.GetLocalControlledEntity();
		if ( !controlledEnt || controlledEnt != m_CharacterOwner)
			return;
		
 		if (mode != EGadgetMode.IN_HAND)
			return;

		ToggleFocused(true);
		SetNotebookMode(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void ModeClear(EGadgetMode mode)
	{	
		// not current player	
		IEntity controlledEnt = SCR_PlayerController.GetLocalControlledEntity();		
		if ( !controlledEnt || controlledEnt != m_CharacterOwner )
		{
			super.ModeClear(mode);
			return;
		}
		
		if (mode == EGadgetMode.IN_HAND)
		{
			if (m_bFocused)
			{
				SetNotebookMode(false);
				ToggleFocused(false);
			}
				
		}
		
		super.ModeClear(mode);
		
	}
			
	//------------------------------------------------------------------------------------------------
	override EGadgetType GetType()
	{
		return EGadgetType.Notebook;
	}
	
};
modded enum EGadgetType
{
	Notebook,
}