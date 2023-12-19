//------------------------------------------------------------------------------------------------
modded class SCR_CampaignSeizingComponent

{
	protected override SCR_Faction EvaluateEntityFaction(IEntity ent)
	{
		if (!m_Base || m_Base.IsHQ() || !m_Base.IsInitialized())
			return null;
		
		SCR_Faction faction = CustEvaluateEntityFaction(ent);
		
		if (!faction)
			return null;
		
		// Players of faction not covering this base with radio signal should not be able to capture or prevent capture
		SCR_CampaignFaction cFaction = SCR_CampaignFaction.Cast(faction);
		
		if (!cFaction && cFaction.GetFactionKey() != "CIV")
			return null;
		
		return faction;
	}
	
	SCR_Faction CustEvaluateEntityFaction(IEntity ent)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(ent);

		if (!char)
			return null;

		if (char.IsInVehicle() && SCR_TerrainHelper.GetHeightAboveTerrain(char.GetOrigin()) > m_iMaximumAltitude)
			return null;

		CharacterControllerComponent charControl = CharacterControllerComponent.Cast(char.FindComponent(CharacterControllerComponent));

		if (charControl && charControl.IsDead())
			return null;

		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());

		if (damageMan && damageMan.GetIsUnconscious())
			return null;

		// Handle after-respawn cooldown
		if (m_fRespawnCooldownPeriod > 0)
		{
			int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(ent);

			if (playerId != 0 && m_mSpawnTimers.Contains(playerId))
			{
				#ifndef AR_CAMPAIGN_TIMESTAMP
				if (m_mSpawnTimers.Get(playerId) > Replication.Time())
				#else
				ChimeraWorld world = GetOwner().GetWorld();
				if (m_mSpawnTimers.Get(playerId).Greater(world.GetServerTimestamp()))
				#endif
					return null;
				else
					m_mSpawnTimers.Remove(playerId)
			}
		}

		SCR_Faction entityFaction = SCR_Faction.Cast(char.GetFaction());

		return entityFaction;
	}
};
