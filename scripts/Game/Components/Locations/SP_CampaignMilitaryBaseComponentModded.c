modded class SCR_CampaignMilitaryBaseComponent
{
	protected IEntity m_eBaseCommander;
	protected IEntity m_eBaseMedic;
	protected IEntity m_eBaseMechanic;
	protected IEntity m_eBaseNavigator;
	protected ref array <IEntity> a_BaseGuards = {};
	protected LightPost m_BaseLightPost;
	
	[Attribute()]
	bool m_bVisited;
	
	protected ref map <Faction, SCR_ECampaignHQRadioComms > m_eFactionRadioCoverage;
	bool IsInhabited()
	{
		if (m_eBaseCommander || m_eBaseMedic || m_eBaseMechanic || m_eBaseNavigator || !a_BaseGuards.IsEmpty())
			return true;
		return false;
	}
	override void OnCapturingFactionChanged()
	{

		m_CapturingFaction = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager()).GetCampaignFactionByIndex(m_iCapturingFaction);

		// Play or stop radio tuning SFX
		if (m_HQRadio)
		{
			SignalsManagerComponent comp = SignalsManagerComponent.Cast(m_HQRadio.FindComponent(SignalsManagerComponent));

			if (comp)
			{
				if (m_CapturingFaction)
					comp.SetSignalValue(comp.AddOrFindSignal(ESTABLISH_ACTION_SIGNAL_NAME), 1);
				else
					comp.SetSignalValue(comp.AddOrFindSignal(ESTABLISH_ACTION_SIGNAL_NAME), 0);
			}
		}

		if (!IsProxy())
		{
			SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();

			if (campaign)
				campaign.GetBaseManager().EvaluateControlPoints();
		}

		SCR_CampaignFeedbackComponent feedback = SCR_CampaignFeedbackComponent.GetInstance();

		if (!feedback)
			return;

		Faction playerFaction = SCR_FactionManager.SGetLocalPlayerFaction();

		if (m_CapturingFaction && GetFaction() == playerFaction)
			feedback.FlashBaseIcon(this, faction: m_CapturingFaction, infiniteTimer: true);
		else
			feedback.FlashBaseIcon(this, changeToDefault: true);
	}
	override void OnHasSignalChanged()
	{
		if (!IsProxy())
		{
			HandleSpawnPointFaction();
			SupplyIncomeTimer(true);
		}

		//if (RplSession.Mode() != RplMode.Dedicated)
			//m_MapDescriptor.HandleMapInfo();

		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();

		if (campaign)
			campaign.GetBaseManager().GetOnSignalChanged().Invoke(this);
	}
	override void OnSpawnPointFactionAssigned(FactionKey faction)
	{
		//if (RplSession.Mode() != RplMode.Dedicated)
			//m_MapDescriptor.HandleMapInfo();
	}
	override void OnServiceBuilt_AfterInit(SCR_ServicePointComponent service)
	{
		// In case the composition has been deleted in the meantime
		if (!service)
			return;

		if (!IsProxy())
		{
			ResourceName delegatePrefab = service.GetDelegatePrefab();
	
			if (delegatePrefab.IsEmpty())
				return;
	
			Resource delegateResource = Resource.Load(delegatePrefab);
	
			if (!delegateResource || !delegateResource.IsValid())
				return;
	
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			service.GetOwner().GetTransform(params.Transform);
			IEntity delegateEntity = GetGame().SpawnEntityPrefab(delegateResource, null, params);
	
			if (!delegateEntity)
				return;
	
			SCR_ServicePointDelegateComponent delegateComponent = SCR_ServicePointDelegateComponent.Cast(delegateEntity.FindComponent(SCR_ServicePointDelegateComponent));
	
			if (!delegateComponent)
				return;
	
			service.SetDelegate(delegateComponent);
			delegateComponent.SetParentBaseId(Replication.FindId(this));
		}

		//if (RplSession.Mode() != RplMode.Dedicated)
			//m_MapDescriptor.HandleMapInfo();
	}
	void RegisterCharacter(IEntity Char, SCR_ECharacterRank position)
	{
		switch (position)
		{
			case SCR_ECharacterRank.MEDIC:
			{
				m_eBaseMedic = Char;
				return;
			}
			break;
			case SCR_ECharacterRank.MECHANIC:
			{
				m_eBaseMechanic = Char;
				return;
			}
			break;
			case SCR_ECharacterRank.COMMANDER:
			{
				m_eBaseCommander = Char;
				return;
			}
			break;
			case SCR_ECharacterRank.NAVIGATOR:
			{
				m_eBaseNavigator = Char;
				
				return;
			}
			break;
		}
		a_BaseGuards.Insert(Char);
	}
	void RegisterPost(LightPost Post)
	{
		m_BaseLightPost = Post;
	}
	LightPost GetBasePost()
	{
		return m_BaseLightPost;
	}
	void GetCharacterOfPost(SCR_ECharacterRank position, out IEntity Char)
	{
		switch (position)
		{
			case SCR_ECharacterRank.MEDIC:
			{
				Char = m_eBaseMedic;
				return;
			}
			break;
			case SCR_ECharacterRank.MECHANIC:
			{
				Char = m_eBaseMechanic;
				return;
			}
			break;
			case SCR_ECharacterRank.COMMANDER:
			{
				Char = m_eBaseCommander;
				return;
			}
			break;
			case SCR_ECharacterRank.NAVIGATOR:
			{
				Char = m_eBaseNavigator;
				return;
			}
			break;
		}
		if (!a_BaseGuards.IsEmpty())
		{
			Char = a_BaseGuards.GetRandomElement();
		}
	}
	void GetGuards(out array <IEntity> BaseGuards)
	{
		BaseGuards = a_BaseGuards;
	}
	void AssignPathToBuild(IEntity Char)
	{
		AIControlComponent comp = AIControlComponent.Cast(Char.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		array <LightPost> posts = {};
		array <string> Mybases = {};
		array<SCR_CampaignMilitaryBaseComponent> basesInRange = {};
		GetBasesInRange(SCR_ECampaignBaseType.BASE, basesInRange);
		if (!basesInRange.IsEmpty())
		{
			foreach (SCR_CampaignMilitaryBaseComponent disbase : basesInRange)
			{
				array <string> bases = {};
				bases.Insert(disbase.GetBaseName());
				bases.Insert(GetBaseName());
				if (!SP_LightPostManager.AreBasesConnected(bases))
				{
					Mybases.Insert(disbase.GetBaseName());
				}
			}
		}
		
		array <string> correctbases = {};	
		correctbases.Insert(GetBaseName());
		correctbases.Insert(Mybases.GetRandomElement());
		SP_LightPostManager.GetLightPolesForBases(correctbases, posts);
		SCR_AIBuildPathBehavior action = new SCR_AIBuildPathBehavior(utility, null, posts);			
				
		utility.AddAction(action);
	}
}