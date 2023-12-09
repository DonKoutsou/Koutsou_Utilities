modded class SCR_AmbientVehicleSpawnPointComponent
{
	[Attribute(defvalue: "2")]
	int m_iAmountofHZtoDamage;
	override Vehicle SpawnVehicle()
	{
		super.SpawnVehicle();
		if (!m_Vehicle)
			return null;
		SCR_VehicleDamageManagerComponent dmgman = SCR_VehicleDamageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleDamageManagerComponent));
		if (dmgman)
		{
			array<HitZone> hitzones = {};
			dmgman.GetAllHitZones(hitzones);
			if (hitzones.Count() > 0)
			{
				for (int i; i < m_iAmountofHZtoDamage; i++)
				{
					HitZone randhit = hitzones.GetRandomElement();
					randhit.SetHealth(Math.RandomFloat(1, (randhit.GetMaxHealth()/2)));
				}
			}
		}
		return m_Vehicle;
	}
}
