//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent
{
//------------------------------------------------------------------------------------------------
	// Scale for effects that become worsened with blood level  
	override float GetResilienceRegenScale()
	{
		if (!m_pBloodHitZone)
			return 1;

		float criticalBloodLevel = m_pBloodHitZone.GetDamageStateThreshold(ECharacterBloodState.UNCONSCIOUS);
		if (criticalBloodLevel >= 1)
			return 0;

		float currentBloodLevel = m_pBloodHitZone.GetDamageStateThreshold(m_pBloodHitZone.GetDamageState());
		if (currentBloodLevel < criticalBloodLevel)
			return 0;

		float resilienceRegenScale = (currentBloodLevel - criticalBloodLevel) / (1 - criticalBloodLevel);
		
		if (IsDamagedOverTime(EDamageType.BLEEDING) && GetIsUnconscious())
			resilienceRegenScale *= m_fUnconsciousRegenerationScale;
		
		if (m_fUnconsciousRegenerationScale == 0 && GetIsUnconscious())
			resilienceRegenScale = 0;
		
		return resilienceRegenScale;
	}
	void SetResilienceRegenScale(float value)
	{
		m_fUnconsciousRegenerationScale = value;
	}


};
