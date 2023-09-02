class SP_CharacterAISmartActionComponentClass: SCR_AISmartActionComponentClass
{
};


class SP_CharacterAISmartActionComponent : SCR_AISmartActionComponent
{
	void SP_CharacterAISmartActionComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Owner = GenericEntity.Cast(ent);
	}
	
};

