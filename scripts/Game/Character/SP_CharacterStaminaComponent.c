modded class SCR_CharacterStaminaComponent
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
