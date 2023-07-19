class KLM_TeleportAction : KLM_BaseTraderAction
{
	[Attribute("", UIWidgets.Coords, params: "inf inf inf purpose=coords space=world", desc: "")]
	vector m_TeleportToWorldPosition;	

	override bool Sell(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// teleport user to wanted world location
		vector mat[4];
		BaseGameEntity char = BaseGameEntity.Cast(pUserEntity);
		if (char)
		{
			char.GetWorldTransform(mat);
			mat[3] = m_TeleportToWorldPosition;
			
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(char.FindComponent(SCR_EditableEntityComponent));
			if (editable)
			{
				editable.SetTransform(mat);
			}
			else
				char.Teleport(mat);
			return true;
		}		
		return false;
	}
}