class SP_NotebookUI : ChimeraMenuBase
{
	ref array <Faction> m_aFactions = {};
	ref map <Faction, int> m_mGoodwillMap;
	
	protected Widget 						m_wRoot;
	protected ButtonWidget			m_wCloseButton;
	protected ButtonWidget 			m_wTextButton;
	protected ButtonWidget			m_wPageF;
	protected ButtonWidget			m_wPageB;
	protected TextWidget			m_wDateText1;
	protected TextWidget			m_wDateText2;
	protected TextWidget			m_wPageNum1;
	protected TextWidget			m_wPageNum2;
	protected ref array <TextWidget>  m_aEntries = {};
	bool ShowText = 1;
	SCR_NotebookGadgetComponent Notebook;
	SP_NotebookComponent Notebookcomp;
	SP_NotebookPage Pagetoshow;
	int Pageindex;
	int day, month, year;
	TimeAndWeatherManagerEntity timeman;
	
	
	void Init(IEntity Owner, IEntity User)
	{
		Notebook = SCR_NotebookGadgetComponent.Cast(Owner.FindComponent(SCR_NotebookGadgetComponent));
		Notebookcomp = SP_NotebookComponent.Cast(Owner.FindComponent(SP_NotebookComponent));
		ChimeraWorld world = GetGame().GetWorld();
		timeman = world.GetTimeAndWeatherManager();
		
		
		m_wRoot = GetRootWidget();
		
		Pageindex = 1;
		
		m_wCloseButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Close"));
		ScriptInvoker onPressedClose = ButtonActionComponent.GetOnAction(m_wCloseButton);
		if (onPressedClose) 
			onPressedClose.Insert(Close);
		GetGame().GetInputManager().AddActionListener("Escape", EActionTrigger.DOWN, CloseNotebook);

		m_wDateText1 = TextWidget.Cast(m_wRoot.FindAnyWidget("w_Date1"));
		m_wDateText2 = TextWidget.Cast(m_wRoot.FindAnyWidget("w_Date2"));
		m_wPageNum1 = TextWidget.Cast(m_wRoot.FindAnyWidget("w_Pagenum1"));
		m_wPageNum2 = TextWidget.Cast(m_wRoot.FindAnyWidget("w_Pagenum2"));
		if (SP_CallendarComponent.Cast(Notebookcomp))
		{
			timeman.GetDate(year, month, day);
			Pageindex = timeman.GetDayIndex(day, month);
			m_wDateText1.SetText(string.Format("%1/%2/%3", day, month, year));
			Notebookcomp.GetPage(Pagetoshow, Pageindex + 1);
			SP_CalendarPage Cpage = SP_CalendarPage.Cast(Pagetoshow);
			Cpage.GetDate().GetDate(day, month);
			m_wDateText2.SetText(string.Format("%1/%2/%3", day, month, year));
		}
		else
		{
			m_wDateText1.SetText(STRING_EMPTY);
			m_wDateText2.SetText(STRING_EMPTY);
		}
		
		
		for (int d = 1; d < 3; d++)
		{
			TextWidget textentry = TextWidget.Cast(m_wRoot.FindAnyWidget("Entry" + d));
			m_aEntries.Insert(textentry);
			textentry.SetText(STRING_EMPTY);
		}
		
		m_wPageF = ButtonWidget.Cast(m_wRoot.FindAnyWidget("w_bPageForward"));
		m_wPageB = ButtonWidget.Cast(m_wRoot.FindAnyWidget("w_bPageBackward"));
		ScriptInvoker onPageF = ButtonActionComponent.GetOnAction(m_wPageF);
		ScriptInvoker onPageB = ButtonActionComponent.GetOnAction(m_wPageB);
		if (onPageF) 
			onPageF.Insert(SwitchPageF);
		if (onPageB) 
			onPageB.Insert(SwitchPageB);
		GetGame().GetInputManager().AddActionListener("PageF", EActionTrigger.DOWN, SwitchPageF);
		GetGame().GetInputManager().AddActionListener("PageB", EActionTrigger.DOWN, SwitchPageB);
		UpdatePage();
	}
	void SwitchPageF(){SwitchPage(false)}
	void SwitchPageB(){SwitchPage(true)}
	//true is back, false is forward
	void SwitchPage(bool direction)
	{
		if (direction)
		{
			if (Pageindex > 2)
				Pageindex = Pageindex - 2;
			/*if (day > 1)
			{
				day = day - 1;
			}
			else
			{
				if (month == 1)
					return;
				month = month - 1;
				day = timeman.GetAmountOfDaysInMonth(month);
			}*/
			
		}
		else
		{
			/*if (day == timeman.GetAmountOfDaysInMonth(month))
			{
				if (month == 12)
					return;
				month = month + 1;
				day = 1;
			}
			else
			{
				day = day + 1;
			}*/
			if (Pageindex < Notebookcomp.GetPageCount() - 2)
				Pageindex = Pageindex + 2;
		}
		UpdatePage();
	}
	void UpdatePage()
	{
		m_wPageNum1.SetText(Pageindex.ToString());
		m_wPageNum2.SetText((Pageindex + 1).ToString());
		//Pageindex = timeman.GetDayIndex(day, month);
		Notebookcomp.GetPage(Pagetoshow, Pageindex);
		if (SP_CallendarComponent.Cast(Notebookcomp))
		{
			SP_CalendarPage CPage = SP_CalendarPage.Cast(Pagetoshow);
			CPage.GetDate().GetDate(day, month);
			m_wDateText1.SetText(string.Format("%1/%2/%3", day, month, SP_CallendarComponent.Cast(Notebookcomp).GetYear()));
			
			
		}
		TextWidget textentry1 = m_aEntries[0];
		TextWidget textentry2 = m_aEntries[1];
		if (Pagetoshow.GetParagraphCount() <= 0)
		{
			textentry1.SetText(STRING_EMPTY);
		}
		else
		{
			string text;
			Pagetoshow.GetComposedText(text);
			textentry1.SetText(text);
		}
		if (Notebookcomp.GetPageCount() < Pageindex + 2)
		{
			m_wDateText2.SetText(STRING_EMPTY);
			textentry2.SetText(STRING_EMPTY);
			return;
		}
		Notebookcomp.GetPage(Pagetoshow, Pageindex + 1);
		if (SP_CallendarComponent.Cast(Notebookcomp))
		{
			SP_CalendarPage CPage = SP_CalendarPage.Cast(Pagetoshow);
			CPage.GetDate().GetDate(day, month);
			m_wDateText2.SetText(string.Format("%1/%2/%3", day, month, SP_CallendarComponent.Cast(Notebookcomp).GetYear()));
		}
		if (Pagetoshow.GetParagraphCount() <= 0)
		{
			textentry2.SetText(STRING_EMPTY);
		}
		else
		{
			string text;
			Pagetoshow.GetComposedText(text);
			textentry2.SetText(text);
		}
		
	}
	void CloseNotebook()
	{
		Notebook.ModeClear(3);
		Close();
	}
	void Update()
	{
	}
	void ToggleText()
	{
		if (ShowText)
			ShowText = 0;
		else if (!ShowText)
			ShowText = 1;
		Update();
	}
	
};
modded enum ChimeraMenuPreset
{
	NotebookMenu,
}