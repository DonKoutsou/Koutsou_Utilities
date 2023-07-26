class SP_SleepAction : ScriptedUserAction
{
	[Attribute()]
	float m_Sleepamount;
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		TimeAndWeatherManagerEntity TnWManager = GetGame().GetTimeAndWeatherManager();
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
		SP_CharacterStatsComponent StatComp = SP_CharacterStatsComponent.Cast(pUserEntity.FindComponent(SP_CharacterStatsComponent));
		StatComp.SetNewHunger(StatComp.GetHunger() - 10.0 * m_Sleepamount);
		StatComp.SetNewThirst(StatComp.GetThirst() - 10.0 * m_Sleepamount);
	}
	event override bool CanBePerformedScript(IEntity user) 
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller)
			return false;
		if (controller.GetControlledEntity() != user)
			return false;
		SP_CharacterStatsComponent StatComp = SP_CharacterStatsComponent.Cast(controller.FindComponent(SP_CharacterStatsComponent));
		float AmountToDeplete = 10.0 * m_Sleepamount;
		if(AmountToDeplete >= StatComp.GetHunger() || AmountToDeplete >= StatComp.GetThirst())
			{
				SetCannotPerformReason("You are not properly hudrated/fed to sleep that much");
				return false;
			}
		return true;
	};
	
}