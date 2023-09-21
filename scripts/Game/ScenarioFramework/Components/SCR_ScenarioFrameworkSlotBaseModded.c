modded class SCR_ScenarioFrameworkSlotBase
{
	//------------------------------------------------------------------------------------------------
	void SelectRandomSlot()
	{
		array<ref EntitySpawnParams> aPosOut = {};
		GetSubSlots(aPosOut);
		
		if (aPosOut.IsEmpty())
		{
			GetOwner().GetWorldTransform(m_SpawnParams.Transform);
		}
		else
		{
			Math.Randomize(-1);
			m_SpawnParams = aPosOut[Math.RandomInt(0, aPosOut.Count())];
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void GetSubSlots(out notnull array<ref EntitySpawnParams> aPosOut)
	{
		IEntity child = GetOwner().GetChildren();
		
		while (child)	
		{
			ref EntitySpawnParams params = new EntitySpawnParams;
			child.GetWorldTransform(params.Transform);
			aPosOut.Insert(params);
			child = child.GetSibling();			
		}
	}
	void SetSpawnedEnt(IEntity entity)
	{
		m_Entity = entity;
	}
}