[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class SP_CharacterTriggerEntityClass : SCR_CharacterTriggerEntityClass
{
};
class SP_CharacterTriggerEntity : SCR_CharacterTriggerEntity
{
	ref array <IEntity> a_charstobleed = new array<IEntity>();

	void AddCharacters(array <IEntity> charstobleed)
	{
		a_charstobleed.Copy(charstobleed);
	}	 
	//------------------------------------------------------------------------------------------------
	override protected event void OnActivate(IEntity ent)
	{
		foreach (IEntity Char : a_charstobleed)
		{
			if (Char)
			{
				SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
				dmg.AddParticularBleeding();
			}
		}
		
	}
	
};
