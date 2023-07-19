class SP_AIGetEmptyCompartment : AITaskScripted
{
	static const string PORT_POSITION 	=	"Position";
	static const string PORT_AGENT 		=	"Agent";
	static const string PORT_VEHICLE 	=	"Vehicle";
	
	[Attribute("0", UIWidgets.CheckBox, "Occupy driver compartment with Group Leader Agent?" )]
	protected bool m_bAllowLeaderAsDriver;
	
	private IEntity m_VehicleEntity;
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(AIAgent owner)
	{
		GetVariableIn(PORT_VEHICLE, m_VehicleEntity);	
	}
	
	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
    {
		if (!m_VehicleEntity)
		{
			ClearVariable(PORT_POSITION);
			return ENodeResult.FAIL;
		}
		
		BaseCompartmentManagerComponent compartmentMan = BaseCompartmentManagerComponent.Cast(m_VehicleEntity.FindComponent(BaseCompartmentManagerComponent));
		if (!compartmentMan)
		{
			return NodeError(this, owner, "Missing compartment manager on IEntity" + m_VehicleEntity.ToString());
		}

		ref array<BaseCompartmentSlot>	compartments = {}, pilotComp = {}, turretComp = {}, cargoComp = {};
		int numOfComp = compartmentMan.GetCompartments(compartments);
		BaseCompartmentSlot compartmentToAlocate;
		bool foundEmptyCompartment;
		
		
		foreach (BaseCompartmentSlot comp : compartments)
		{
			if (!comp.AttachedOccupant() && comp.IsCompartmentAccessible())
			{
				if (PilotCompartmentSlot.Cast(comp)) 
					pilotComp.Insert(comp);
				else if (TurretCompartmentSlot.Cast(comp))
					turretComp.Insert(comp);
				else
				{
					cargoComp.Insert(comp);
					compartmentToAlocate = comp;
				}
					
				foundEmptyCompartment = true;
			}
		}
		
		if (!foundEmptyCompartment)
		{
			ClearVariable(PORT_POSITION);
			return ENodeResult.FAIL;
		}
		
		if (compartmentToAlocate)
		{
			PrintFormat("Here %1 for %2", compartmentToAlocate, owner);
			SetVariableOut(PORT_POSITION, compartmentToAlocate);				
			return ENodeResult.SUCCESS;
		}
		
		ClearVariable(PORT_POSITION);
		return ENodeResult.FAIL;
	}
	
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		PORT_VEHICLE,
		PORT_AGENT
	};
	override TStringArray GetVariablesIn()
	{
		return s_aVarsIn;
	}
	
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsOut = {
		PORT_POSITION
	};
	override TStringArray GetVariablesOut()
    {
        return s_aVarsOut;
    }
	
	//------------------------------------------------------------------------------------------------
	override bool VisibleInPalette()
    {
        return true;
    }
	
	//------------------------------------------------------------------------------------------------
	override string GetOnHoverDescription()
	{
		return "Returns type of next usable compartment. Compartments are allocated in GetIn activity. The compartments are selected with priority pilot>turret>cargo unless AIAgent is leader";
	}
};