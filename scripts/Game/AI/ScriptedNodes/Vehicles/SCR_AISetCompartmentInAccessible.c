class SCR_AISetCompartmentInAccessible : AITaskScripted
{
	static const string Vehicle_PORT = "VehicleIn";
	
	protected bool m_bAbortDone;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------
	override void OnEnter(AIAgent owner)
	{
		m_bAbortDone = false;
	}
	
	//----------------------------------------------------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		IEntity vehicle;
		BaseCompartmentSlot compartment;
		if (!GetVariableIn(Vehicle_PORT, vehicle))
			return ENodeResult.FAIL;
		compartment = BaseCompartmentSlot.Cast(vehicle.FindComponent(BaseCompartmentSlot));
		compartment.SetReserved(owner.GetControlledEntity());
		return ENodeResult.SUCCESS;
	}
	
	//----------------------------------------------------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		Vehicle_PORT
	};
	
	override TStringArray GetVariablesIn()
	{
		return s_aVarsIn;
	};
	
	override bool VisibleInPalette()
	{
		return true;
	};
	
	override string GetOnHoverDescription()
	{
		return "SetCompartmnetAccessible: makes the compartment accessible by other AIs OnAbort";
	};
	
	override bool CanReturnRunning()
	{
		return true;
	};
};