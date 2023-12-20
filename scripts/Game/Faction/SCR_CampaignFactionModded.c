
modded class SCR_CampaignFaction
{
	[Attribute(defvalue:"{98DD1D36C5B67A1C}prefabs/Items/EntryPapers/Entry_Papers_Base.et")]
	ResourceName m_FactionPapersPrefab;
	
	[Attribute("-1")]
	int m_iStartingPlayerGoodwill;

	//Relations map
	protected ref map<Faction, int> m_FriendlyMap;
	
	//Playergoodwill
	protected float m_fPGoodwillValue;
	
	//---------------------------------------------------------------------//
	protected ref ScriptInvoker s_OnRelationLow = new ref ScriptInvoker();
	protected ref ScriptInvoker s_OnRelationHigh = new ref ScriptInvoker();

	ScriptInvoker OnRelationDropped()
	{
		return s_OnRelationLow;
	}
	ScriptInvoker OnRelationRaised()
	{
		return s_OnRelationHigh;
	}
	//Adjust relation
	void AdjustRelation(Faction faction, int amount)
	{
		if(faction == this)
			return;
		if(!m_FriendlyMap.IsEmpty())
		{
			int relation;
			m_FriendlyMap.Find(faction, relation);
			m_FriendlyMap.Set(faction, relation + amount);
			if (GetFactionRep(faction) <= -500 && faction.IsFactionFriendly(this))
				s_OnRelationLow.Invoke(faction, this);
			if (GetFactionRep(faction) > -500 && faction.IsFactionEnemy(this))
				s_OnRelationHigh.Invoke(faction, this);
		}
	}
	//Set Relation
	void AdjustRelationAbs(Faction faction, int amount)
	{
		if(faction == this)
			return;
		if(m_FriendlyMap)
		{
			int relation;
			m_FriendlyMap.Find(faction, relation);
			m_FriendlyMap.Set(faction, amount);
			if (GetFactionRep(faction) <= -500 && faction.IsFactionFriendly(this))
				s_OnRelationLow.Invoke(faction, this);
			if (GetFactionRep(faction) > -500 && faction.IsFactionEnemy(this))
				s_OnRelationHigh.Invoke(faction, this);
		}
	}
	
	int GetFactionRep(Faction fact)
	{
		int rep;
		if (m_FriendlyMap.Contains(fact))
		{
			rep = m_FriendlyMap.Get(fact);
		}
		return rep;
	}
	//Fill warray with friendly factions
	void GetFriendlyFactions2(out array<Faction> friendlyfactions)
	{
		friendlyfactions = new array<Faction>();
		foreach (Faction fact : m_FriendlyFactions)
		{
			friendlyfactions.Insert(fact);
		}
	}
	//Fill array with enemy factions
	void GetEnemyFactions(out array<Faction> enemyfactions)
	{
		SCR_FactionManager factman = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		array<Faction> faction = {};
		factman.GetFactionsList(faction);
		foreach (Faction fact : faction)
		{
			if (IsFactionEnemy(fact))
				enemyfactions.Insert(fact);
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
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
		{
			//~ Still make sure faction is friendly towards itself	
			if (m_bFriendlyToSelf)
			{
				SetFactionFriendly(this);
				m_FriendlyMap.Insert(this, 2000);
			}
			
			Print("'SCR_Faction' is trying to set friendly factions but no SCR_FactionManager could be found!", LogLevel.ERROR);
		}
		else 
		{
			//~ Make sure faction is friendly towards itself
			if (m_bFriendlyToSelf)
			{
				factionManager.SetFactionsFriendly(this, this);
				m_FriendlyMap.Insert(this, 2000);
			}
			else
				m_FriendlyMap.Insert(this, -2000);
			
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
					{
						
						continue;
					}
					//~ Assign as friendly
					factionManager.SetFactionsFriendly(this, faction);
					m_FriendlyMap.Insert(faction, 1000);
				}
			}
		}
		array <Faction> factions = new array <Faction>();
		factionManager.GetEnemyFactions(this, factions);
		foreach (Faction enemf : factions)
		{
			AdjustRelationAbs(enemf, -1000);
		}
	}
	void SetPlayerGoodwill(int value)
	{
		m_fPGoodwillValue = value;
	}
	void AdjustPlayerGoodwill(int amount, SP_EGoodWillReason reason)
	{	
		amount = amount * -1;
		m_fPGoodwillValue = m_fPGoodwillValue - amount;
		if (m_fPGoodwillValue <= -500)
		{
			IEntity player = GetGame().GetPlayerController().GetControlledEntity();
			FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));
			FactionComp.SetAffiliatedFactionByKey("RENEGADE");
			SCR_HintManagerComponent.GetInstance().ShowCustom("Your Goodwill has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
			return;
		}
	}
	int GetPlayerGoodwill()
	{
		return m_fPGoodwillValue;
	}
};
enum SP_EGoodWillReason
{
	KILL,
	STEAL,
	TASK,
}