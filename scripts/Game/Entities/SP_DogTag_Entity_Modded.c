modded class DogTagEntity
{

	override event protected void EOnFrame(IEntity owner, float timeSlice)
	{
		if(!charname)
		{
			IEntity parent = owner.GetParent();

			parent = ChimeraCharacter.Cast(parent);
			if(parent)
			{
				SCR_CharacterIdentityComponent IdentityComponent = SCR_CharacterIdentityComponent.Cast(parent.FindComponent(SCR_CharacterIdentityComponent));
				SCR_CharacterRankComponent RankComponent = SCR_CharacterRankComponent.Cast(parent.FindComponent(SCR_CharacterRankComponent));
				if(IdentityComponent && RankComponent)
				{
					Identity ID = IdentityComponent.GetIdentity();
					if(ID)
					{
						charname = RankComponent.GetCharacterRankNameShort(parent) + " " + ID.GetFullName();
					}
				}
			}
		}
		if (charname)
		{
			ClearEventMask(EntityEvent.FRAME);
		}
	};
}
