//------------------------------------------------------------------------------------------------
//! modded version for to be used with the inventory 2.0 
class SCR_ReadItemAction : ScriptedUserAction
{
	override event void PerformAction (IEntity pOwnerEntity, IEntity pUserEntity)
	{
		MenuManager menuManager = g_Game.GetMenuManager();
		MenuBase myMenu = menuManager.OpenMenu(ChimeraMenuPreset.NotebookMenu);
		GetGame().GetInputManager().ActivateContext("NotebookContext");
		SP_NotebookUI NotebookUI = SP_NotebookUI.Cast(myMenu);
		NotebookUI.Init(GetOwner(), pUserEntity);
	}
	override bool CanBePerformedScript(IEntity user)
 	{
		
		return true;
 	}
};