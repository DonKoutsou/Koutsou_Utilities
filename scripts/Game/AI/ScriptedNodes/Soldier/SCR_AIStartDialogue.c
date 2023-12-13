class SCR_AIStartDialogue: AITaskScripted
{
	[Attribute("Who are you ?")]
	string m_IntroString;

	private bool m_bAllowLeave = 1;
	
	protected static const string ENTITY_PORT = "EntityIn";
	protected static const string ALLOWLEAVE_PORT = "AllowLeaveIn";
	
	//-----------------------------------------------------------------------------------------------
	override bool VisibleInPalette() {return true;}
	
	//-----------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		ENTITY_PORT, ALLOWLEAVE_PORT
	};
	override array<string> GetVariablesIn()
	{
		return s_aVarsIn;
	}
	
	//-----------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		ChimeraCharacter Chimera = ChimeraCharacter.Cast(owner.GetControlledEntity());
		CharacterControllerComponent Controller = Chimera.GetCharacterController();
		
		IEntity ent;
		
		if (!GetVariableIn(ENTITY_PORT,ent))
			return ENodeResult.FAIL;
		GetVariableIn(ALLOWLEAVE_PORT, m_bAllowLeave);
		
		SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(ent.FindComponent(SCR_EditableEntityComponent));
		if (editable)
		{
			vector mat[4];
			mat[3] = ent.GetOrigin();
			SCR_Math3D.LookAt(ent.GetOrigin(), Chimera.GetOrigin(), vector.Up, mat);
			editable.SetTransform(mat);
		}
		
		ChimeraCharacter DChimera = ChimeraCharacter.Cast(ent);
		MenuManager menumanager = GetGame().GetMenuManager();
		MenuBase myMenu = menumanager.OpenMenu(ChimeraMenuPreset.DialogueMenu);
		GetGame().GetInputManager().ActivateContext("DialogueMenuContext");
		DialogueUIClass DiagUI = DialogueUIClass.Cast(myMenu);
		DS_DialogueComponent DiagComp = DS_DialogueComponent.GetInstance();
		//DiagComp.IntroductionSting(Chimera, DChimera);
		if (m_IntroString)
		{
			array<BaseInfoDisplay> infoDisplays = {};
			GetGame().GetPlayerController().GetHUDManagerComponent().GetInfoDisplays(infoDisplays);
			foreach (BaseInfoDisplay baseInfoDisplays : infoDisplays)
			{
				SCR_DialogueWidget DialogueDisplay = SCR_DialogueWidget.Cast(baseInfoDisplays);
				if (!DialogueDisplay)
					continue;
		
				DialogueDisplay.SetTarget(Chimera);
				DialogueDisplay.SetText(m_IntroString);
				DialogueDisplay.ShowInspectCasualtyWidget(Chimera);
			}
		}
		DiagUI.Init(Chimera, DChimera);
		DiagUI.UpdateEntries(Chimera, DChimera, m_bAllowLeave);
		
		return ENodeResult.SUCCESS;
    }
}