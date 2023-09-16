class TaskDateInfo
{
	int m_iDay;
	int m_iMonth;
	int m_iYear;
	void TaskDateInfo(int Day, int Month, int Year)
	{
		m_iDay = Day;
		m_iMonth = Month;
		m_iYear = Year;
	}
	void GetDate(out int Day, out int Month, out int Year)
	{
		Day = m_iDay;
		Month = m_iMonth;
		Year = m_iYear;
	}
	bool HasPassed(TaskDateInfo info)
	{
		int Day;
		int Month;
		int Year;
		info.GetDate(Day, Month, Year);
		if (Year > m_iYear)
			return true;
		if (Month > m_iMonth)
			return true;
		if (Day > m_iDay)
			return true;
		return false;
		
	}
	float CalculateTimeDifferance(TaskDateInfo DateInfo)
	{
		float dif;
		int Day;
		int Month;
		int Year;
		DateInfo.GetDate(Day, Month, Year);
		if (Day > m_iDay)
		{
			for (int i, count = Day - m_iDay; i < count; i++)
			{
				dif += 24;
			}
		}
		if (Month > m_iMonth)
		{
			for (int i, count = Month - m_iMonth; i < count; i++)
			{
				dif += 24 * TimeAndWeatherManagerEntity.GetAmountOfDaysInMonth(m_iMonth);
			}
		}
		if (Year > m_iYear)
		{
			for (int i, count = Year - m_iYear; i < count; i++)
			{
				dif += 8760;
			}
		}
		return dif;
	}
}