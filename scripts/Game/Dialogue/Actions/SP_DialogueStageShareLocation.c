[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageShareLocationAction : DS_BaseDialogueStageAction
{
	protected int m_iGridSizeX;
	protected int m_iGridSizeY;
	
	protected const float angleA = 0.775;
	protected const float angleB = 0.325;

	protected int GetGridIndex(int x, int y)
	{
		return 3*y + x;
	}
};
[BaseContainerProps(configRoot: true)]
class SP_DialogueDirectionHint
{
	[Attribute(desc: "Description of given quad. Order: SW, S, SE, W, C, E, NW, N, NE", params: "MaxSize=9")]
	protected ref array<string> m_aQuadHints;
	
	[Attribute("#AR-MapLocationHint_PlayerPositionIslandName")]
	protected string m_sPlayerPositionHint;
	
	LocalizedString GetPlayerPositionHint()
	{
		return m_sPlayerPositionHint;
	}
	
	string GetQuadHint(int index)
	{
		if (!m_aQuadHints.IsIndexValid(index))
			return LocalizedString.Empty;

		return m_aQuadHints[index];
	}
};
[BaseContainerProps(configRoot:true)]
class DateEventContainer : BaseEventContainer
{
	[Attribute(desc: "text override ", defvalue : "I asked around for directions and i've been told i am")]
	string text;
	override string GetString(IEntity Player, IEntity Char)
	{
		DS_DialogueComponent Diag = DS_DialogueComponent.GetInstance();
	 	string loc = Diag.GetCharacterLocation(Player, true);
		return text + " " + loc;
	}
}