modded class SCR_AISmartActionComponent : AISmartActionComponent
{
	[Attribute()]
	ref SmartActionTest ActionTest;

	SCR_CharacterDamageManagerComponent m_pDamageManager;
	bool NeedsTest()
	{
		if (ActionTest)
			return true;
		return false;
	}
	bool RunTest(IEntity User)
	{
		return ActionTest.TestAction(GetOwner(), User);
	}
};
class DecoratorScripted_TestSmartAction : DecoratorScripted
{
	
	protected override bool TestFunction(AIAgent owner)
	{
		SCR_AISmartActionComponent SmartAct;
		GetVariableIn("SmartAction", SCR_AISmartActionComponent);
		if (SmartAct.NeedsTest())
		{
			return SmartAct.RunTest(owner.GetControlledEntity());
		}
		return true;
	}
	
	protected override bool VisibleInPalette()
	{
		return true;
	}	
	
	protected static ref TStringArray s_aVarsIn = {
		"SmartAction"
	};
	protected override TStringArray GetVariablesIn()
	{
		return s_aVarsIn;
	}
};
