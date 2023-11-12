class SP_SleepAction : ScriptedUserAction
{
	[Attribute()]
	float m_Sleepamount;
	
	SCR_FadeInOutEffect FadeInOutEffect;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.Cast(playerController.GetHUDManagerComponent());
		if (!hudManager)
			return;
		FadeInOutEffect = SCR_FadeInOutEffect.Cast(hudManager.FindInfoDisplay(SCR_FadeInOutEffect));
		FadeInOutEffect.FadeOutEffect(false, 1000);
		GetGame().GetCallqueue().CallLater(Fadeout, 1000, false);
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
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		statComponent.SetNewHunger(statComponent.GetHunger() - 3.6 * m_Sleepamount);
		statComponent.SetNewThirst(statComponent.GetThirst() - 7.2 * m_Sleepamount);
		statComponent.SetNewEnergy(statComponent.GetEnergy() + 10 *  m_Sleepamount);
		SCR_ChimeraCharacter Char = SCR_ChimeraCharacter.Cast(pUserEntity);
		SP_CallendarComponent Callendar = Char.GetCallendar();
		if (Callendar)
		{
			Callendar.RecordEvent(string.Format("I slept for %1 hours to regain some streanth", m_Sleepamount));
		}
	}
	void Fadeout()
	{
		FadeInOutEffect.FadeOutEffect(false, 10);
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