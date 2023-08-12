modded class SCR_AIDecoTestDoorIsOpen
{
	override protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			if (!m_doorComponent)
				m_doorComponent = DoorComponent.Cast(controlled.FindComponent(DoorComponent));
			if (!m_doorComponent)
				return false;
			if (m_doorComponent.GetControlValue() > 0.2 || m_doorComponent.GetControlValue() < - 0.2)
				return true;
			else
				return false;
		}
		return false;
	}
};