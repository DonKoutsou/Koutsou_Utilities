class SP_FactionRelationTableAction : ScriptedUserAction
{
	//------------------------------------------------------------------//
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		MenuBase myMenu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.FactionRelationTable);
		GetGame().GetInputManager().ActivateContext("Faction_Menu_Context");
		FactionTableUI FactionUI = FactionTableUI.Cast(myMenu);
		FactionUI.Init(pOwnerEntity, pUserEntity);
		//FactionUI.UpdateEntries(pOwnerEntity, pUserEntity);
	}
	override event void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
	};
	//------------------------------------------------------------------//
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//----------------------------------------------------------------------------------
	override bool CanBroadcastScript() 
	{ 
		return false; 
	};
}