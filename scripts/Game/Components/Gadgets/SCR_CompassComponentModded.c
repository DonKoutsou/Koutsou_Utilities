modded class SCR_CompassComponent : SCR_GadgetComponent
{			
	float timesince;
	override protected void UpdateNeedleDirection(float timeSlice)
	{
		float northDirection = Math.RandomFloat(0, 360);
		float magneticTorque = Math.Sin((northDirection - m_fNeedleAngle) * Math.DEG2RAD);

		float acceleration = magneticTorque * m_fNeedleAccelerationConstant;
		float drag = m_fNeedleVelocity * m_fNeedleDragConstant;

		float cappedTimeSlice = Math.Min(timeSlice, 1);

		m_fNeedleVelocity += (acceleration - drag) * cappedTimeSlice;
		m_fNeedleAngle += m_fNeedleVelocity * cappedTimeSlice;

		m_fNeedleAngle = SCR_Math.fmod(m_fNeedleAngle, 360);
		if (m_fNeedleAngle > 180) m_fNeedleAngle -= 360;

		m_SignalManager.SetSignalValue(m_PrefabData.m_iSignalNeedle, m_fNeedleAngle);
	}
	
};