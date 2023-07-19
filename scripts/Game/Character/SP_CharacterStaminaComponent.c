[ComponentEditorProps(category: "GameScripted/Character", description: "Stamina component for character.", icon: HYBRID_COMPONENT_ICON)]
class SP_CharacterStaminaComponentClass : SCR_CharacterStaminaComponentClass
{
};

class SP_CharacterStaminaComponent : SCR_CharacterStaminaComponent
{
	protected float m_fStaminaDrain;
	protected ref ScriptInvoker m_OnStaminaDrain;

	event override void OnStaminaDrain(float pDrain)
	{
		if (m_OnStaminaDrain && m_fStaminaDrain != pDrain)
		{
			m_fStaminaDrain = pDrain;
			m_OnStaminaDrain.Invoke(pDrain);
		}
	}

	ScriptInvoker GetOnStaminaDrain()
	{
		if (!m_OnStaminaDrain)
			m_OnStaminaDrain = new ScriptInvoker();

		return m_OnStaminaDrain;
	}
	
};
