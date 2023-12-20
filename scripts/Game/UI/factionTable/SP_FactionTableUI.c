class FactionTableUI : ChimeraMenuBase
{
	ref array <Faction> m_aFactions = {};
	ref map <Faction, int> m_mGoodwillMap;
	
	protected Widget 						m_wRoot;
	protected ButtonWidget			m_wCloseButton;
	protected ButtonWidget 			m_wTextButton;
	bool ShowText = 1;
	
	void Init(IEntity Owner, IEntity User)
	{
		SCR_FactionManager factionMan = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		factionMan.GetFactionsList(m_aFactions);
		m_wRoot = GetRootWidget();
		m_wTextButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("TextButton"));
		m_wCloseButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Close"));
		ScriptInvoker onPressed = ButtonActionComponent.GetOnAction(m_wTextButton);
		ScriptInvoker onPressedClose = ButtonActionComponent.GetOnAction(m_wCloseButton);
		if (onPressed) 
			onPressed.Insert(ToggleText);
		if (onPressedClose) 
			onPressedClose.Insert(Close);
		SetValuesVertically();
		GetGame().GetInputManager().AddActionListener("Escape", EActionTrigger.DOWN, Close);
		GetGame().GetInputManager().AddActionListener("SwitchText", EActionTrigger.DOWN, ToggleText);
		
	}
	void Update()
	{
		SetValuesVertically();
	}
	void ToggleText()
	{
		if (ShowText)
			ShowText = 0;
		else if (!ShowText)
			ShowText = 1;
		Update();
	}
	
	void SetValuesVertically()
	{
		for (int i = 0; i < m_aFactions.Count(); i++)
		{
			SetFactionNames(i);
			SetUpRow(i);
		}
	}
	void SetUpRow(int Row)
	{
		SCR_CampaignFaction SCRFact = SCR_CampaignFaction.Cast(m_aFactions[Row]);
		for (int i = 0; i < m_aFactions.Count() + 1; i++)
		{
			string widgetname = string.Format("UG_%1_%2", Row, i);
			TextWidget widget = TextWidget.Cast(m_wRoot.FindAnyWidget(widgetname));
			if (!widget)
				continue;
			if (i == 0)
			{
				int goodwill = SCRFact.GetPlayerGoodwill();
				widget.SetText(goodwill.ToString());
				if (goodwill <= -500)
				{
					widget.SetColor(Color.Red);
					if (ShowText)
						widget.SetText("Minimal");
				}
				else if (goodwill >= 1000)
				{
					widget.SetColor(Color.Green);
					if (ShowText)
						widget.SetText("Friend");
				}
				else
				{
					widget.SetColor(Color.Yellow);
					if (ShowText)
						widget.SetText("Regular");
				}
			}
			else
			{
				int factrep = SCRFact.GetFactionRep(m_aFactions[i - 1]);
				widget.SetText(factrep.ToString());
				if (factrep <= -500)
				{
					widget.SetColor(Color.Red);
					if (ShowText)
						widget.SetText("Enemy");
				}
				else if (factrep > 1000)
				{
					widget.SetColor(Color.Green);
					if (ShowText)
						widget.SetText("Friendly");
				}
				else
				{
					widget.SetColor(Color.Yellow);
					if (ShowText)
						widget.SetText("Neuteral");
				}
			}
			
		}
	}
	void SetFactionNames(int value)
	{
		string widgetnameV = string.Format("UG_V_Fact_%1", value);
		string widgetnameH = string.Format("UG_H_Fact_%1", value);
		TextWidget widgetV = TextWidget.Cast(m_wRoot.FindAnyWidget(widgetnameV));
		TextWidget widgetH = TextWidget.Cast(m_wRoot.FindAnyWidget(widgetnameH));
		if (widgetV)
			widgetV.SetText(m_aFactions[value].GetFactionKey());
		if (widgetH)	
			widgetH.SetText(m_aFactions[value].GetFactionKey());
	}
	
};
modded enum ChimeraMenuPreset
{
	FactionRelationTable,
}