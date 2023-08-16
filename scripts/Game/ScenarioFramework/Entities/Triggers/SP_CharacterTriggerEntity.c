[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class SP_CharacterTriggerEntityClass : SCR_CharacterTriggerEntityClass
{
};
class SP_CharacterTriggerEntity : SCR_CharacterTriggerEntity
{
	ref array <IEntity> a_charstobleed = new array<IEntity>();
	
	[Attribute()]
	bool AutoPlacementCorrection;
	
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
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		if (AutoPlacementCorrection)
		{
			CheckPlacement();
		}
		super.OnQueryFinished(bIsEmpty);
		
		if (bIsEmpty)
			return;
		
		if (!IsMaster())
			return;

		if (!m_bInitSequenceDone)
			return;
		
		ActivationPresenceConditions();
		CustomTriggerConditions();
		HandleTimer();
		
		if (m_bEnableAudio)
			HandleAudio();

		if (m_bTriggerConditionsStatus && m_fTempWaitTime <= 0)
			FinishTrigger(this);
	}
	void CheckPlacement()
	{
			if (vector.Distance(GetOrigin(), a_charstobleed[0].GetOrigin()) > 5)
				SetOrigin(a_charstobleed[0].GetOrigin());
	}
	
};
