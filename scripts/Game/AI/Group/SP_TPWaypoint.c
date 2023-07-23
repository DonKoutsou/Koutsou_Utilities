class SP_TPWaypointClass: SCR_AIWaypointClass
{
};

[BaseContainerProps()]
class SP_TPWaypointPreset
{
	
};

class SP_TPWaypoint : SCR_AIWaypoint
{
	[Attribute("0", UIWidgets.Object, "Fast init - units will be spawned on their defensive locations")];
	private bool m_bFastInit;
	
	[Attribute("", UIWidgets.EditBox, "Preset name, only informative. Switch using index.")];
	private string m_sName;
	
	[Attribute("true", UIWidgets.CheckBox, "Use turrets?")];
	bool m_bAllowCloseSpawn;
	
	[Attribute("", UIWidgets.Auto, "List tags to search in the preset")];
	ref array<string> m_aTagsForSearch;	
	
	private int m_iCurrentDefendPreset;
	 
	//----------------------------------------------------------------------------------------
	array<string> GetTagsForSearch()
	{
		return m_aTagsForSearch;
	}
	void SetTagsForSearch(array<string> tags)
	{
		m_aTagsForSearch.Clear();
		m_aTagsForSearch.Copy(tags);
	}
	//----------------------------------------------------------------------------------------
	bool GetAllowCloseSpawn()
	{
		return m_bAllowCloseSpawn;
	}
	void SetAllowCloseSpawning(bool value)
	{
		m_bAllowCloseSpawn = value;
	}
	//----------------------------------------------------------------------------------------
	bool GetFastInit()
	{
		return m_bFastInit;
	}
	
	//----------------------------------------------------------------------------------------
	void SetFastInit(bool isFastInit)
	{
		m_bFastInit = isFastInit;
	}
};