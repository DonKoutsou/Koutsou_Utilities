class LightPostClass: GameEntityClass{};
class LightPost: GameEntity
{
	[Attribute("0",UIWidgets.ComboBox, enums : ParamEnumArray.FromEnum(SP_BaseEn))]
	ref array <SP_BaseEn> m_aConnectingBases;
	
	[Attribute("2")]
	int m_iPathDirections;
	
	private ref ScriptInvoker OnLightPostBuilt;
	
	SP_BaseTask m_TaskMarker;
	
	
	void GetConnectingBases(out array <string> bases)
	{
		if (m_aConnectingBases.IsEmpty())
			return;
		foreach(SP_BaseEn basename : m_aConnectingBases)
		{
			bases.Insert(SP_BaseNames.Get(basename));
		}
	}
	
	void SpawnTaskMarkers(IEntity Assignee)
	{
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
		foreach (SP_BaseEn basename : m_aConnectingBases)
		{
			bases.Insert(SP_BaseNames.Get(basename));
		}
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
		if (m_aConnectingBases.IsEmpty())
			return false;
		foreach (SP_BaseEn Basename : m_aConnectingBases)
		{
			if (!bases.Contains( SP_BaseNames.Get( Basename )) )
				return false;
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
	//Destructor
	/*void ~LightPost()
	{
		SP_LightPostManager.GetInstane().UnRegisterPost(this);
	};*/
	
}
class SP_LightPostManagerClass : ScriptComponentClass
{
	
};
[BaseContainerProps(configRoot: true)]
class SP_LightPostManager : ScriptComponent
{
	static SP_LightPostManager m_instance;
	static ref array <LightPost> m_aLightposts;
	static ref array <string> m_aConnectedBases;
	
	static SP_LightPostManager GetInstane()
	{
		return m_instance;
	}
	void OnBasesConnected(array <string> ConnectedBases)
	{
		foreach(string base : ConnectedBases)
		{
			m_aConnectedBases.Insert(base);
		}
	}
	void OnPostBuilt(LightPost post)
	{
		array <string> bases = {};
		post.GetConnectingBases(bases);
		if (AreBasesConnected(bases))
		{
			OnBasesConnected(bases);
		}
	}
	
	void RegisterPost(LightPost Post)
	{
		if (!m_aLightposts.Contains(Post))
			m_aLightposts.Insert(Post);
		Post.GetOnBuilt().Insert(OnPostBuilt);
	}
	void UnRegisterPost(LightPost Post)
	{
		if (m_aLightposts.Contains(Post))
			m_aLightposts.RemoveItem(Post);
	}
	static void GetLightPolesForBases( array <string> bases, out array <LightPost> Posts )
	{
		for (int i; i < m_aLightposts.Count(); i++)
		{
			if (m_aLightposts[i].ConnectBases(bases))
			{
				Posts.Insert(m_aLightposts[i]);
			}
		}
	}
	static bool AreBasesConnected(array <string> bases)
	{
		array <LightPost> Posts = {};
		GetLightPolesForBases(bases, Posts);
		if (Posts.IsEmpty())
			return false;
		for (int i; i < Posts.Count(); i++)
		{
			if (!Posts[i].IsBuilt())
				return false;
		}
		return true;
	}
	static void EnableBuildingPreviews(array <string> bases)
	{
		array <LightPost> Posts = {};
		GetLightPolesForBases(bases, Posts);
		if (Posts.IsEmpty())
			return;
		for (int i; i < Posts.Count(); i++)
		{
			Posts[i].SetVisible();
			SCR_PathPointMapDescriptorComponent mapdesc = SCR_PathPointMapDescriptorComponent.Cast(Posts[i].FindComponent(SCR_PathPointMapDescriptorComponent));
			mapdesc.Item().SetVisible(true);
		}
		return;
	}
	void SP_LightPostManager(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_aLightposts = {};
		m_aConnectedBases = {};
		m_instance = this;
	}
	void ~SP_LightPostManager()
	{
		m_aLightposts.Clear();
		m_aConnectedBases.Clear();
	}
}