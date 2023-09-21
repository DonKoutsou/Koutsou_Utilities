class TaskDayTimeInfo
{
	ref TimeContainer m_fTimeOfDay;
	ref TaskDateInfo Date;
	
	void TaskDayTimeInfo(float Time, int Day, int Month, int Year)
	{
		Date = new TaskDateInfo(Day, Month, Year);
		m_fTimeOfDay = TimeContainer.FromTimeOfTheDay(Time);
	}
	void GetDate(out float Time, out int Day, out int Month, out int Year)
	{
		Date.GetDate(Day, Month, Year);
		m_fTimeOfDay = TimeContainer.FromTimeOfTheDay(Time);
		Time = m_fTimeOfDay.ToTimeOfTheDay();
	}
	TimeContainer GetTime()
	{
		return m_fTimeOfDay;
	};
	TaskDateInfo GetDateInfo()
	{
		return Date;
	};
	bool HasPassed(TaskDayTimeInfo DayInfo)
	{
		if (Date.HasPassed(DayInfo.GetDateInfo()))
			return true;
		TimeContainer Time = DayInfo.GetTime();
		if (Time.ToTimeOfTheDay() > m_fTimeOfDay.ToTimeOfTheDay())
			return true;
		return false;
	}
	float CalculateTimeDifferance(TaskDayTimeInfo Info)
	{
		float dif;
		float Time = Info.GetTime().ToTimeOfTheDay();
		dif += m_fTimeOfDay.ToTimeOfTheDay() - Time;
		if (Info.GetDateInfo().HasPassed(Date))
		{
			int DateDiff = Info.GetDateInfo().CalculateTimeDifferance(Date);
			dif += DateDiff;
		}
		return dif;
	}
	static TaskDayTimeInfo FromTimeOfTheDay()
	{
		ChimeraWorld world = GetGame().GetWorld();
		TimeAndWeatherManagerEntity timenw = world.GetTimeAndWeatherManager();
		float Time = timenw.GetTime().ToTimeOfTheDay();
		int Day, Month, Year;
		timenw.GetDate(Year, Month, Day);
		return new TaskDayTimeInfo(Time ,Day, Month, Year);
	}
	static TaskDayTimeInfo FromPointInFuture(int Ammount, ETaskTimeLimmit TimeLimmit)
	{
		ChimeraWorld world = GetGame().GetWorld();
		TimeAndWeatherManagerEntity timenw = world.GetTimeAndWeatherManager();
		float Time = timenw.GetTime().ToTimeOfTheDay();
		int Day, Month, Year;
		timenw.GetDate(Year, Month, Day);
		if (TimeLimmit == ETaskTimeLimmit.HOURS)
		{
			Time += Ammount;
		}
		while (Time > 24)
		{
			Time -= 24;
			Day += 1;
		}
		while (Day > TimeAndWeatherManagerEntity.GetAmountOfDaysInMonth(Month))
		{
			Day -= TimeAndWeatherManagerEntity.GetAmountOfDaysInMonth(Month);
			Month += 1;
		}
		return new TaskDayTimeInfo(Time ,Day, Month, Year);
	}
};