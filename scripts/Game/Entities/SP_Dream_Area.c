class DreamWorldClass: GameEntityClass{};
class DreamWorld: GameEntity
{
	static vector m_vOrigLoc[4];
	static IEntity m_CharInDream;
	
	static void TeleportToDream(IEntity Char)
	{
		m_vOrigLoc[3] = Char.GetOrigin();
		m_CharInDream = Char;
		SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(Char.FindComponent(SCR_EditableEntityComponent));
		if (editable)
		{
			vector mat[4];
			editable.SetTransform(mat);
		}
	};
	static void TeleportOutOfDream(IEntity Char)
	{
		SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(Char.FindComponent(SCR_EditableEntityComponent));
		if (editable)
		{
			editable.SetTransform(m_vOrigLoc);
		}
	};
}