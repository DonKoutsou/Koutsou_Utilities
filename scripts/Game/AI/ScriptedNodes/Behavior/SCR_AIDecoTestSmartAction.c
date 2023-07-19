 class SCR_AIDecoTestHasSmartAction : DecoratorTestScripted
{
	private SCR_AISmartActionComponent m_SmartActionComponent;
	
	//------------------------------------------------------------------------------------------------
	protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			m_SmartActionComponent = SCR_AISmartActionComponent.Cast(controlled.FindComponent(SCR_AISmartActionComponent));
			if (m_SmartActionComponent)
				return true;
		}
		return false;
	}
};