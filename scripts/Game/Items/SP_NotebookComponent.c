class SP_NotebookComponentClass : ScriptGameComponentClass
{
	
}
class SP_NotebookComponent : ScriptGameComponent
{
	[Attribute()]
	ref array <ref SP_NotebookPage> a_Pages;
	
	
	ref array <ref SP_NotebookPage> m_aPages = {};
	
	
	void AddPage(SP_NotebookPage Page)
	{
		m_aPages.Insert(Page);
	}
	override event protected void EOnInit(IEntity owner)
	{
		ChimeraWorld game =  GetGame().GetWorld();
		if (!game)
			return;
		TimeAndWeatherManagerEntity weather = game.GetTimeAndWeatherManager();
		
		SP_NotebookPage nullpage = new SP_NotebookPage(0);
		m_aPages.Insert(nullpage);
		
		for (int i; i < a_Pages.Count(); i++)
		{
			a_Pages[i].m_iPagenumber = i;
			m_aPages.Insert(a_Pages[i]);
		}	
	};
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	};
	void GetPage(out SP_NotebookPage page, int Index)
	{
		page = m_aPages.Get(Index);
	};
	int GetPageCount()
	{
		return m_aPages.Count();
	}
}
class SP_CallendarComponentClass : SP_NotebookComponentClass
{
}
class SP_CallendarComponent : SP_NotebookComponent
{
	[Attribute()]
	int YearNumber;
	
	int m_iyear;
	override event protected void EOnInit(IEntity owner)
	{
		ChimeraWorld game =  GetGame().GetWorld();
		if (!game)
			return;
		
		TimeAndWeatherManagerEntity weather = game.GetTimeAndWeatherManager();
		if (YearNumber)
			m_iyear = YearNumber;
		else
			m_iyear = weather.GetYear();
		SP_NotebookPage nullpage = new SP_NotebookPage(0);
		m_aPages.Insert(nullpage);
		
		for (int i = 1; i < 13; i++)
		{
			int daysinmonth = weather.GetAmountOfDaysInMonth(i);
			for (int d = 1; d < daysinmonth + 1; d++)
			{
				
				if (a_Pages)
				{
					bool addednew;
					for (int p; p < a_Pages.Count(); p++)
					{
						SP_CalendarPage Page = SP_CalendarPage.Cast(a_Pages.Get(p));
						int Day;
						int Month;
						Page.GetDate().GetDate(Day, Month);
						if (Day == d && Month == i)
						{
							m_aPages.Insert(Page);
							a_Pages.RemoveItem(Page);
							addednew = 1;
							break;
						}
					}
					if (addednew)
						continue;
				}
				SP_CalendarPage newpage = new SP_CalendarPage(m_aPages.Count(), d, i);
				m_aPages.Insert(newpage);
			}
		}	
	};
	void RecordEvent(string EventDescription)
	{
		ChimeraWorld game =  GetGame().GetWorld();
		if (!game)
			return;
		
		TimeAndWeatherManagerEntity weather = game.GetTimeAndWeatherManager();
		int day, month, year;
		weather.GetDate(year, month, day);
		int DayIndex = weather.GetDayIndex(day, month);
		SP_CalendarPage Page = SP_CalendarPage.Cast(m_aPages.Get(DayIndex));
		if (!Page)
			return;
		Page.AddParagraph(EventDescription);
	}
	int GetYear()
	{
		return m_iyear;
	}
}
[BaseContainerProps(configRoot:true)]
class SP_NotebookPage
{
	[Attribute()]
	ref array <string> m_aParagraphs;
	
	int m_iPagenumber;
	
	void SP_NotebookPage(int Pagenumber)
	{
		if (!m_iPagenumber)
		{
			m_iPagenumber = Pagenumber
		}
		
	}
	int GetParagraphCount()
	{
		if (!m_aParagraphs)
			return 0;
		return m_aParagraphs.Count();
	}
	void GetParagraph(out string entrytext, int index)
	{
		entrytext = m_aParagraphs.Get(index);	
	}
	void GetComposedText(out string Text)
	{
		for (int i; i < m_aParagraphs.Count(); i++)
		{
			if (!Text)
			{
				Text = m_aParagraphs[i];
				continue;
			}
			Text = Text + "\n" + m_aParagraphs[i];
		}
	}
	int GetPageNumber()
	{
		return m_iPagenumber;
	}
	void AddParagraph(string Text)
	{
		if (!m_aParagraphs)
			m_aParagraphs = {};
		m_aParagraphs.Insert(Text);
	}
}
[BaseContainerProps(configRoot:true)]
class SP_CalendarPage : SP_NotebookPage
{
	[Attribute()]
	ref PageDateInfo Date;
	void SP_CalendarPage(int Pagenumber, int Day, int Month)
	{
		if (!Date)
		{
			Date = new PageDateInfo(Day, Month);
		}
		m_iPagenumber = Pagenumber
	}
	PageDateInfo GetDate()
	{
		return Date;
	}
}
[BaseContainerProps(configRoot:true)]
class PageDateInfo
{
	[Attribute()]
	int m_iDay;
	[Attribute()]
	int m_iMonth;
	void PageDateInfo(int Day, int Month)
	{
		if (!m_iDay)
			m_iDay = Day;
		if (!m_iMonth)
			m_iMonth = Month;
	}
	void GetDate(out int Day, out int Month)
	{
		Day = m_iDay;
		Month = m_iMonth;
	}
}
class SP_NotepadPredicate : InventorySearchPredicate
{
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SP_NotebookComponent Notebook = SP_NotebookComponent.Cast(item.FindComponent(SP_NotebookComponent));
		if(Notebook)
		{
			return true;
		}
		return false;
	}
}
class SP_CallendarPredicate : InventorySearchPredicate
{
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SP_CallendarComponent Callendar = SP_CallendarComponent.Cast(item.FindComponent(SP_CallendarComponent));
		if(Callendar)
		{
			return true;
		}
		return false;
	}
}