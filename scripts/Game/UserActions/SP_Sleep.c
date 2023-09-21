class SP_SleepAction : ScriptedUserAction
{
	[Attribute()]
	float m_Sleepamount;
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ChimeraWorld world = pOwnerEntity.GetWorld();
		TimeAndWeatherManagerEntity TnWManager = world.GetTimeAndWeatherManager();
		int TimeToWakeUP = TnWManager.GetTime().m_iHours + m_Sleepamount;
		int DayToWakeUp = TnWManager.GetDay();
		if(TimeToWakeUP > 24)
		{
			TimeToWakeUP = DayToWakeUp - 24;
			DayToWakeUp = DayToWakeUp + 1;
			TnWManager.SetTimeOfTheDay(TimeToWakeUP, true);
			TnWManager.SetDate(TnWManager.GetYear(), TnWManager.GetMonth(), DayToWakeUp, true);
		}
		else
		{
			TnWManager.SetTimeOfTheDay(TimeToWakeUP, true);
		}
		PlayerController playerController = GetGame().GetPlayerController();
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		statComponent.SetNewHunger(statComponent.GetHunger() - 3.6 * m_Sleepamount);
		statComponent.SetNewThirst(statComponent.GetThirst() - 7.2 * m_Sleepamount);
		statComponent.SetNewEnergy(statComponent.GetEnergy() + 10 *  m_Sleepamount);
	}
	event override bool CanBePerformedScript(IEntity user) 
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller)
			return false;
		if (controller.GetControlledEntity() != user)
			return false;
		SP_CharacterStatsComponent StatComp = SP_CharacterStatsComponent.Cast(controller.FindComponent(SP_CharacterStatsComponent));
		float AmountToDeplete = 3600 * m_Sleepamount;
		if(AmountToDeplete * 0.001 >= StatComp.GetHunger() || AmountToDeplete * 0.002 >= StatComp.GetThirst())
			{
				SetCannotPerformReason("You are not properly hudrated/fed to sleep that much");
				return false;
			}
		return true;
	};
	
}