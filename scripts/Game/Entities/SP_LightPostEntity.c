class LightPostClass: GameEntityClass{};
class LightPost: GameEntity
{
	[Attribute(defvalue : "0", UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	ref array <SP_BaseEn> m_aConnectingBases;
	
	[Attribute("",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_EColor))]
	ref array <SP_EColor> m_aColors;
	
	[Attribute(defvalue : "0", UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	SP_BaseEn m_BaseName;
	
	[Attribute()]
	bool m_bRegisterInBase;
	
	[Attribute("2")]
	int m_iPathDirections;
	
	private ref ScriptInvoker OnLightPostBuilt;
	
	SP_BaseTask m_TaskMarker;
	
	[Attribute()]
	bool m_bShowDebugBaseNames;
	
	bool HasSameColors(LightPost post)
	{
		if (m_aColors.IsEmpty())
			return false;
		foreach (SP_EColor color : m_aColors)
		{
			if (post.m_aColors.Contains(color))
				return true;
		}
		return false;
	}
	
	Color GetColor()
	{
		Color final = new Color(0, 0, 0, 1.0);
		if (m_aColors.IsEmpty())
			return null;
		foreach(int col : m_aColors)
		{
			final.Lerp(SP_ColorNames.Get(col), 0.5);
		}		
		return final;
	}
	array <Color> GetColors()
	{
		 array <Color> colors = {};
			foreach(int col : m_aColors)
			{
				colors.Insert(SP_ColorNames.Get(col));
			}		
			return colors;
	}
	
	void GetConnectingBases(out array <string> bases)
	{
		if (m_aConnectingBases.IsEmpty())
			return;
		
		for (int i = 0; i < m_aConnectingBases.Count();i++)
		{
			if (SP_BaseNames.Get(m_aConnectingBases[i]) == STRING_EMPTY)
				continue;
			bases.Insert(SP_BaseNames.Get(m_aConnectingBases[i]));
		}
	}
	
	void SpawnTaskMarkers(IEntity Assignee)
	{
		if (IsBuilt())
			return;
		SCR_AutoSpawnLogic logic = SCR_AutoSpawnLogic.Cast(SCR_RespawnSystemComponent.GetInstance().GetSpawnLogic());
		if (!logic.m_bAllowTaskMarkers)
			return;
		//Get task resource
		Resource Marker = Resource.Load( "{304847F9EDB0EA1B}prefabs/Tasks/SP_BaseTask.et" );
		//setup spawn params
		EntitySpawnParams PrefabspawnParams = EntitySpawnParams();
		//Getaffiliation of Assignee to add to marker
		FactionAffiliationComponent Aff = FactionAffiliationComponent.Cast( Assignee.FindComponent( FactionAffiliationComponent ) );
		//get spawn location from target
		GetWorldTransform(PrefabspawnParams.Transform);
		//spawn marker
		m_TaskMarker = SP_BaseTask.Cast(GetGame().SpawnEntityPrefab(Marker, GetGame().GetWorld(), PrefabspawnParams));
			
		//set info
		m_TaskMarker.SetTitle( "Build" );
		m_TaskMarker.SetDescription( "Buld lightpost." );
		m_TaskMarker.SetTarget( this );
		m_TaskMarker.SetTargetFaction( Aff.GetAffiliatedFaction() );
			
		//assign to player
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(Assignee);
		SCR_BaseTaskExecutor assignee = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerID);
		m_TaskMarker.AddAssigneecustom(assignee, 0);
	}
	ref ScriptInvoker GetOnBuilt()
	{
		if (!OnLightPostBuilt)
			OnLightPostBuilt = new ScriptInvoker();
		return OnLightPostBuilt;
	}
	
	protected void OnBuilt()
	{
		if (OnLightPostBuilt)
			OnLightPostBuilt.Invoke(this);
		
	}
	
	bool m_bBuilt;
	
	bool IsBuilt()
	{
		return m_bBuilt;
	}
	void SetBuilt(bool built)
	{
		m_bBuilt = built;
		if (m_TaskMarker)
			m_TaskMarker.Finish(true);
		OnBuilt();
		SetUpLink();
	}
	void SetUpLink()
	{
		array <string> bases = {};
		GetConnectingBases(bases);
		array <LightPost> Posts = {};
		SP_LightPostManager.GetLightPolesForBases(bases, Posts);

		array <LightPost> ClosePosts = {};
		for (int i; i < m_iPathDirections; i++)
		{
			float dist;
			LightPost closestpost
			foreach (LightPost post : Posts)
			{
				if (post == this)
					continue;
				if (!dist)
				{
					dist = vector.Distance(GetOrigin(), post.GetOrigin());
					closestpost = post;
				}
				else if (dist > vector.Distance(GetOrigin(), post.GetOrigin()))
				{
					dist = vector.Distance(GetOrigin(), post.GetOrigin());
					closestpost = post;
				}
			}
			Posts.RemoveItem(closestpost);
			ClosePosts.Insert(closestpost);
		}
		SCR_PathPointMapDescriptorComponent mapdesc = SCR_PathPointMapDescriptorComponent.Cast(FindComponent(SCR_PathPointMapDescriptorComponent));
		mapdesc.Item().SetBaseType(EMapDescriptorType.MDT_TREE); 
		for (int i; i < m_iPathDirections; i++)
		{
			if (!ClosePosts[i].IsBuilt())
				continue;
			SCR_PathPointMapDescriptorComponent mapdesc2 = SCR_PathPointMapDescriptorComponent.Cast(ClosePosts[i].FindComponent(SCR_PathPointMapDescriptorComponent));
			mapdesc.Item().LinkTo(mapdesc2.Item());
		}
		/*
		float dist;
		float dist2;
		LightPost closestpost, closestpost2;
		foreach (LightPost post : Posts)
		{
			if (post == this)
				continue;
			if (!dist)
			{
				dist = vector.Distance(GetOrigin(), post.GetOrigin());
				closestpost = post;
			}
			else if (dist > vector.Distance(GetOrigin(), post.GetOrigin()))
			{
				dist = vector.Distance(GetOrigin(), post.GetOrigin());
				closestpost = post;
			}
		}
		Posts.RemoveItem(closestpost);
		dist = 0;
		foreach (LightPost post : Posts)
		{
			if (post == this)
				continue;
			if (!dist)
			{
				dist = vector.Distance(GetOrigin(), post.GetOrigin());
				closestpost2 = post;
			}
			else if (dist > vector.Distance(GetOrigin(), post.GetOrigin()))
			{
				dist = vector.Distance(GetOrigin(), post.GetOrigin());
				closestpost2 = post;
			}
		}
		SCR_ServicePointMapDescriptorComponent mapdesc = SCR_ServicePointMapDescriptorComponent.Cast(FindComponent(SCR_ServicePointMapDescriptorComponent));
		SCR_ServicePointMapDescriptorComponent mapdesc2 = SCR_ServicePointMapDescriptorComponent.Cast(closestpost.FindComponent(SCR_ServicePointMapDescriptorComponent));
		SCR_ServicePointMapDescriptorComponent mapdesc3 = SCR_ServicePointMapDescriptorComponent.Cast(closestpost2.FindComponent(SCR_ServicePointMapDescriptorComponent));
		if (closestpost.IsBuilt())
		{
			mapdesc.Item().LinkTo(mapdesc2.Item());
		}
		if (closestpost2.IsBuilt())
		{
			mapdesc.Item().LinkTo(mapdesc3.Item());
		}*/
		
		
	}
	void UpdateLinks()
	{
		
	}
	bool ConnectBases(array <string> bases)
	{
		array <string> basestocheck = {};
		GetConnectingBases(basestocheck);
		if (basestocheck.IsEmpty())
			return false;
		for (int i = 0; i < basestocheck.Count();i++)
		{
			if (bases.Contains( basestocheck[i] ))
				return true;
		}
		return true;
	}
	void SetVisible()
	{
		EntitySlotInfo slot = SlotManagerComponent.Cast(FindComponent(SlotManagerComponent)).GetSlotByName("Lantern");
		slot.GetAttachedEntity().SetFlags(EntityFlags.VISIBLE);
		SetFlags(EntityFlags.VISIBLE);
	}
	void LightPost(IEntitySource src, IEntity parent)
	{
		SP_LightPostManager man = SP_LightPostManager.GetInstane();
		if (!man)
			return;
		man.RegisterPost(this);
		
		//GetGame().GetCallqueue().CallLater(SetVisible, 2000, false);
	};
	void RegisterPostToBase()
	{
		SCR_CampaignMilitaryBaseManager baseman = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();

		SCR_CampaignMilitaryBaseComponent base = baseman.GetNamedBase(SP_BaseNames.Get( m_BaseName ) );
		
		base.RegisterPost(this);
	}
	//Destructor
	void ~LightPost()
	{
		SP_LightPostManager man = SP_LightPostManager.GetInstane();
		if (!man)
			return;
		man.UnRegisterPost(this);
	};
	
}