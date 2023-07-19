class SP_CharacterAISmartActionComponentClass: SCR_AISmartActionComponentClass
{
};


class SP_CharacterAISmartActionComponent : SCR_AISmartActionComponent
{
	override void EOnInit(IEntity owner)
	{
		SetActionAccessible(true);
	}
};

