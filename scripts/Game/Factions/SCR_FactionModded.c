//------------------------------------------------------------------------------------------------
modded class SCR_Faction
{
	protected ref map<Faction, int> m_FriendlyMap;
	
	protected ref ScriptInvoker s_OnRelationLow = new ref ScriptInvoker();
	protected ref ScriptInvoker s_OnRelationHigh = new ref ScriptInvoker();
	
	void AdjustRelation(Faction faction, int amount)
	{
		if(faction == this)
			return;
		if(m_FriendlyMap)
		{
			int relation;
			m_FriendlyMap.Find(faction, relation);
			m_FriendlyMap.Set(faction, relation + amount);
			if (GetFactionRep(faction) <= -50 && faction.IsFactionFriendly(this))
				s_OnRelationLow.Invoke(faction, this);
			if (GetFactionRep(faction) > -50 && faction.IsFactionEnemy(this))
				s_OnRelationHigh.Invoke(faction, this);
		}
	}
	void AdjustRelationAbs(Faction faction, int amount)
	{
		if(faction == this)
			return;
		if(m_FriendlyMap)
		{
			int relation;
			m_FriendlyMap.Find(faction, relation);
			m_FriendlyMap.Set(faction, amount);
			if (GetFactionRep(faction) <= -50 && faction.IsFactionFriendly(this))
				s_OnRelationLow.Invoke(faction, this);
			if (GetFactionRep(faction) > -50 && faction.IsFactionEnemy(this))
				s_OnRelationHigh.Invoke(faction, this);
		}
	}
	ScriptInvoker OnRelationDropped()
	{
		return s_OnRelationLow;
	}
	ScriptInvoker OnRelationRaised()
	{
		return s_OnRelationHigh;
	}
	int GetFactionRep(Faction fact)
	{
		if (fact == this)
			return 100;
		int rep;
		if (m_FriendlyMap.Contains(fact))
		{
			rep = m_FriendlyMap.Get(fact);
		}
		return rep;
	}
	void GetFriendlyFactions2(out array<Faction> friendlyfactions)
	{
		friendlyfactions = new array<Faction>();
		foreach (Faction fact : m_FriendlyFactions)
		{
			friendlyfactions.Insert(fact);
		}
	}
	override void Init(IEntity owner)
	{
		super.Init(owner);
		
		if (SCR_Global.IsEditMode()) 
			return;
		if (!m_FriendlyMap)
			m_FriendlyMap = new ref map<Faction, int>();
		//~ Set faction friendly
		SP_FactionManager factionManager = SP_FactionManager.Cast(GetGame().GetFactionManager());
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
					m_FriendlyMap.Insert(faction, 100);
				}
			}
		}
		array <Faction> factions = new array <Faction>();
		factionManager.GetEnemyFactions(this, factions);
		foreach (Faction enemf : factions)
		{
			AdjustRelationAbs(enemf, -100);
		}
	}
};
