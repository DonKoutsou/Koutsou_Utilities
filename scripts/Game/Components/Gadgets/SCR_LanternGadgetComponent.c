[EntityEditorProps(category: "GameScripted/Gadgets", description: "Lantern", color: "0 0 255 255")]
class SCR_LanternGadgetComponentClass: SCR_FlashlightComponentClass
{
};

//------------------------------------------------------------------------------------------------
class SCR_LanternGadgetComponent : SCR_FlashlightComponent
{	
	[Attribute(defvalue : "2000.000")]
	protected float m_fShouldBeAliveFor;

	
	float alivetime;
	
	override protected void EnableLight()
	{	
		super.EnableLight();
		ActivateGadgetFlag();
	}
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{		
		if (!m_CharController || m_CharController.IsDead())	// deactivate frame on death without turning the light off
		{
			DeactivateGadgetFlag();
			return;
		}
		alivetime -= timeSlice;
		
		if (alivetime <= 0)
			DisableLight();
		
	}
	override void EOnInit(IEntity owner)
	{		
		super.EOnInit(owner);
		alivetime = m_fShouldBeAliveFor;
	}

};