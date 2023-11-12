//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_ContainerActionTitle()]
class SP_ScenarioFrameworkActionLockOrUnlockVehicle : SCR_ScenarioFrameworkActionBase
{
	[Attribute(desc: "Slot which spawns the vehicle")];
	protected ref SCR_ScenarioFrameworkGet		m_Getter;

	[Attribute(defvalue: "true", desc: "If set to true, it will lock the vehicle, if set to false it will unlock the vehicle")];
	protected bool m_bLock;

	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity object)
	{
		if (!CanActivate())
			return;

		if (!m_Getter)
			return;

		SCR_ScenarioFrameworkParam<IEntity> entityWrapper = SCR_ScenarioFrameworkParam<IEntity>.Cast(m_Getter.Get());
		if (!entityWrapper)
			return;

		IEntity entity = IEntity.Cast(entityWrapper.GetValue());
		if (!entity)
			return;

		if (!Vehicle.Cast(entity))
			return;

		SCR_VehicleSpawnProtectionComponent spawnProtectionComponent = SCR_VehicleSpawnProtectionComponent.Cast(entity.FindComponent(SCR_VehicleSpawnProtectionComponent));
		if (!spawnProtectionComponent)
			return;

		if (m_bLock)
		{
			spawnProtectionComponent.SetProtectOnlyDriverSeat(false);
			spawnProtectionComponent.SetReasonText("Locked");
			spawnProtectionComponent.SetVehicleOwner(-2);
		}
		else
		{
			spawnProtectionComponent.SetProtectOnlyDriverSeat(true);
			spawnProtectionComponent.SetReasonText("#AR-Campaign_Action_CannotEnterVehicle-UC");
			spawnProtectionComponent.ReleaseProtection();
		}
	}
}