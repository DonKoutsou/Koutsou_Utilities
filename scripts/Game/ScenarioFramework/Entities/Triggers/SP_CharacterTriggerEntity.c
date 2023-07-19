[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class SP_CharacterTriggerEntityClass : SCR_CharacterTriggerEntityClass
{
};
class SP_CharacterTriggerEntity : SCR_CharacterTriggerEntity
{
	[Attribute("")]
	ResourceName m_Commanders;
	
	[Attribute("")]
	protected string m_DirectorKey;
	
	SP_AIDirector Owner;
	
	override protected event void OnActivate(IEntity ent)
	{
		 
		if (!m_bInitSequenceDone)
			return;

		if (!m_aEntitiesInside.Contains(ent))
			m_aEntitiesInside.Insert(ent);

		ActivationPresenceConditions();
		CustomTriggerConditions();
		HandleTimer();
		
		
		if (m_fTempWaitTime <= 0)
		{
			//Owner.SpawnCommander(m_Commanders);	
			m_fTempWaitTime = m_fActivationCountdownTimer;
		}
		
	}
	override protected void EOnInit(IEntity owner)
	{
		//TODO: we need a time to spawn entities inside the trigger, but we don't want to activate the trigger yet.
		//It will be done better by knowing the entities inside the trigger on its creation
		GetGame().GetCallqueue().CallLater(SetInitSequenceDone, 1000);
		SetOwnerFaction(m_sOwnerFactionKey);
		ChimeraWorld world = GetGame().GetWorld();
		Owner = SP_AIDirector.Cast(GetWorld().FindEntityByName("SP_AIDirector_" + m_DirectorKey));
		if (world)
			m_MusicManager = world.GetMusicManager();
	}
	
};
