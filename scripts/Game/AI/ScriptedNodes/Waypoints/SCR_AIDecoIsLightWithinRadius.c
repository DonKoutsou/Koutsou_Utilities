class SCR_AIDecoIsLightWithinRadius : DecoratorScripted
{
	static const string WAYPOINT_PORT = "WaypointIn";
	static const string LOCATION_PORT = "LocationOut";
	
	//---------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		LOCATION_PORT
	};
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	//---------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		WAYPOINT_PORT
	};
	override TStringArray GetVariablesIn()
    {
        return s_aVarsIn;
    }
	
	//---------------------------------------------------------------------------------------------------	
	protected override bool TestFunction(AIAgent owner)
	{
		AIWaypoint waypoint;
		SCR_AIGroupUtilityComponent guc;
		
		AIGroup group = AIGroup.Cast(owner);
		if (!group)
			return false;
		
		if ( !GetVariableIn(WAYPOINT_PORT,waypoint) )
			waypoint = group.GetCurrentWaypoint();
		if (! waypoint )
		{
			Print("Node IsEnemyWithinRadius executed without valid waypoint!");
			return false;
		};
		guc = SCR_AIGroupUtilityComponent.Cast(owner.FindComponent(SCR_AIGroupUtilityComponent));
		if ( !guc )	
		{
			Print("Node IsEnemyWithinRadius executed on owner without group utility component!");
			return false;
		}
		return true;
	}	
	
	//---------------------------------------------------------------------------------------------------
	protected override bool VisibleInPalette()
	{
		return true;
	}
	
	protected override string GetOnHoverDescription()
	{
		return "Decorator that test that at least one detected enemy is within completion radius of waypoint, current waypoint is used if none is provided";
	}
};