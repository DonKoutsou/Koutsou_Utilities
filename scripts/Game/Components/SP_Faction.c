/*modded class SCR_Faction 
{
	
	protected map<SCR_Faction, int> relationmap;
	
	override void Init(IEntity owner)
	{
		super.Init(owner);
		
		if (SCR_Global.IsEditMode()) 
			return;
		
		//~ Set faction friendly
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
		{
			//~ Still make sure faction is friendly towards itself	
			if (m_bFriendlyToSelf)
				SetFactionFriendly(this);
			
			Print("'SCR_Faction' is trying to set friendly factions but no SCR_FactionManager could be found!", LogLevel.ERROR);
		}
		else 
		{
			//~ Make sure faction is friendly towards itself
			if (m_bFriendlyToSelf)
				factionManager.SetFactionsFriendly(this, this);
			
			//~ On init friendly factions assigning
			if (!m_aFriendlyFactionsIds.IsEmpty())
			{			
				SCR_Faction faction;
				
				//~ Set each given faction ID as friendly
				foreach (string factionId : m_aFriendlyFactionsIds)
				{
					faction = SCR_Faction.Cast(factionManager.GetFactionByKey(factionId));
					
					if (!faction)
					{
						Print(string.Format("'SCR_Faction' is trying to set friendly factions on init but '%1' is not a valid SCR_Faction!", factionId), LogLevel.ERROR);
						continue;
					}
					
					//~ Don't set self as friendly
					if (faction == this)
						continue;
					
					//~ Assign as friendly
					factionManager.SetFactionsFriendly(this, faction);
				}
			}
		}
		
		//~ Move catalogs to map for quicker processing
		foreach (SCR_EntityCatalog entityCatalog: m_aEntityCatalogs)
		{
			//~ Ignore duplicate catalog types
			if (m_mEntityCatalogs.Contains(entityCatalog.GetCatalogType()))
				continue;
			
			m_mEntityCatalogs.Insert(entityCatalog.GetCatalogType(), entityCatalog);
		}
		
		//~ Clear array as no longer needed
		m_aEntityCatalogs = null;
	}
}