modded class SCR_AIGroup
{
		//------------------------------------------------------------------------------------------------
	override void OnWaypointAdded(AIWaypoint wp)
	{
		SCR_AIWaypoint scwp =	SCR_AIWaypoint.Cast(wp);
		if (scwp)
		{
			AIFormationComponent form = AIFormationComponent.Cast(FindComponent(AIFormationComponent));
			form.SetFormation(scwp.m_sFormation);
		} 
		Event_OnWaypointAdded.Invoke(wp);
	}
}
