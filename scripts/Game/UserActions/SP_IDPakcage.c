class SP_IDPackageAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DS_DialogueComponent DiagComp = DS_DialogueComponent.GetInstance();
		SP_PackageComponent Pac = SP_PackageComponent.Cast(pOwnerEntity.FindComponent(SP_PackageComponent));
		string OName;
		string DName;
		string LocName;
		FactionKey faction;
		Pac.GetInfo(OName, DName, LocName);
		DiagComp.DoAnouncerDialogue("This package is from " + OName + ". " + "Its ment for " + DName + ". " + "I was told he is located at " + LocName);
	}
	override bool CanBePerformedScript(IEntity user)
	{
		if(GetOwner().FindComponent(SP_PackageComponent))
		{
			return true;
		}
		return false;
	}
}
		