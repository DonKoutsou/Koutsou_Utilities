class SP_LightPostManagerClass : ScriptComponentClass
{
	
};
[BaseContainerProps(configRoot: true)]
class SP_LightPostManager : ScriptComponent
{
	static SP_LightPostManager m_instance;
	static ref array <LightPost> m_aLightposts;
	static ref array <ref Path> m_aPaths;
	static ref array <string> m_aConnectedBases;
	
	[Attribute()]
	bool m_benableDebug;
	
	
	
	int pathnum = 1;
	float cooldown = 20;
	static SP_LightPostManager GetInstane()
	{
		return m_instance;
	}
	void GetPath(out array <Path> paths, string BaseFrom, string BaseTo)
	{
		if (m_aPaths.IsEmpty())
			return;
		foreach (Path path : m_aPaths)
		{
			if (path.ConnectsBases(BaseFrom, BaseTo))
			{
				paths.Insert(path);
			}
		}
	};
	void GetPathsForBase(out array <Path> paths, string base)
	{
		if (m_aPaths.IsEmpty())
			return;
		foreach (Path path : m_aPaths)
		{
			if (path.IsForBase(base))
			{
				paths.Insert(path);
			}
		}
	};
	static void GetConnectedPosts(LightPost post, out array <LightPost> ConnectedPost)
	{
		//array <string> bases = {};
		//foreach (SP_BaseEn basename : post.m_aConnectingBases)
		//{
		//	bases.Insert(SP_BaseNames.Get(basename));
		//}
		//array <LightPost> Posts = {};
		//SP_LightPostManager.GetLightPolesForBases(bases, Posts);

		for (int i; i < post.m_iPathDirections; i++)
		{
			float dist;
			LightPost closestpost
			foreach (LightPost dispost : m_aLightposts)
			{
				if (ConnectedPost.Contains(dispost))
					continue;
				if (dispost == post)
					continue;
				if (!dispost.HasSameColors(post))
					continue;
				if (!dist)
				{
					dist = vector.Distance(post.GetOrigin(), dispost.GetOrigin());
					closestpost = dispost;
				}
				else if (dist > vector.Distance(post.GetOrigin(), dispost.GetOrigin()))
				{
					dist = vector.Distance(post.GetOrigin(), dispost.GetOrigin());
					closestpost = dispost;
				}
			}
			//Posts.RemoveItem(closestpost);
			ConnectedPost.Insert(closestpost);
		}
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
	void RegisterPostsToBases()
	{
		foreach (LightPost post : m_aLightposts)
		{
			if (post.m_bRegisterInBase)
				post.RegisterPostToBase();
		}
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
			if (m_aLightposts[i] == null)
				continue;
			if (m_aLightposts[i].ConnectBases(bases))
			{
				Posts.Insert(m_aLightposts[i]);
			}
		}
	}
	void GeneratePaths()
	{
		RegisterPostsToBases();
		SCR_CampaignMilitaryBaseManager baseman = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
		
		
		for (int i = 1; i < 46; i++)
		{
			SP_BaseEn BaseFrom = i;
			SCR_CampaignMilitaryBaseComponent basefr = baseman.GetNamedBase(SP_BaseNames.Get( BaseFrom ));
			if (!basefr)
			{
				Print(string.Format("PATH CREATION FAILED: creation of path from %1 was unsuccesfull. Reason : %1 doesent exist", SP_BaseNames.Get( BaseFrom )));
				continue;
			}
			for (int v = 1; v < 46; v++)
			{
				if (i == v)
					continue;
				SP_BaseEn BaseTo = v;
				SCR_CampaignMilitaryBaseComponent baseto = baseman.GetNamedBase(SP_BaseNames.Get( BaseTo ));
				if (!baseto)
				{
					Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Reason : %2 doesent exist", SP_BaseNames.Get( BaseFrom ), SP_BaseNames.Get( BaseTo )));
					continue;
				}
				CreatePath(basefr, baseto);
			}
		}
	}
	void PrintPaths(float timeslice)
	{
		cooldown = cooldown - timeslice;
		if (m_aPaths.IsEmpty())
			return;
		if (pathnum >= m_aPaths.Count())
			pathnum = 1;

		
		Path pathtoprint = m_aPaths[pathnum];
		for (int i; i < pathtoprint.Count(); i++)
		{
			if (i == 0)
				continue;
			Shape.CreateArrow(pathtoprint[i - 1].GetOrigin(), pathtoprint[i].GetOrigin(), 10, pathtoprint[i].GetColor().PackToInt(), ShapeFlags.ONCE | ShapeFlags.NOZBUFFER);
		}
		if (cooldown <= 0)
		{
			cooldown = 10;
			pathnum ++;
			Path pathtoprint2 = m_aPaths[pathnum];
			Print(string.Format("Printing path from %1 to %2", SCR_StringHelper.Translate( pathtoprint2.bases[0] ), SCR_StringHelper.Translate( pathtoprint2.bases[1])));
		}
		
	}
	void CreatePath(SCR_CampaignMilitaryBaseComponent basefr, SCR_CampaignMilitaryBaseComponent baseto)
	{
		LightPost PostFrom = basefr.GetBasePost();
		LightPost PostTo = baseto.GetBasePost();

		Path path = new Path();
		path.AddBase(basefr.GetBaseName());
		path.AddBase(baseto.GetBaseName());
		if (!PostFrom)
		{
			Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Reason : %1 doesent have a post", basefr.GetBaseName(), baseto.GetBaseName()));
			return;
		}
			
		if (!PostTo)
		{
			Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Reason : %2 doesent have a post", basefr.GetBaseName(), baseto.GetBaseName()));
			return;
		}
		path.Insert(PostFrom);
		
		
		array <LightPost> Decisions = {};
		array <LightPost> DecisionsBlacklist = {};
		while (!path.Contains(PostTo))
		{
			LightPost currentpost = path.Get(path.Count() - 1);
			
			LightPost nextpost;
			array <LightPost> ConnectedPosts = {};
			GetConnectedPosts(currentpost, ConnectedPosts);
			bool basesondist;
			foreach (LightPost post : ConnectedPosts)
			{
				if (path.Contains(post))
					continue;
				if (DecisionsBlacklist.Contains(post))
					continue;
				if (post.m_aConnectingBases.Contains(SP_BaseNames.Convert(baseto)) || post.m_iPathDirections > 2)
				{
					if (nextpost)
					{
						float dist1 = vector.Distance(post.GetOrigin(), baseto.GetOwner().GetOrigin());
						float dist2 = vector.Distance(nextpost.GetOrigin(), baseto.GetOwner().GetOrigin());
						float least = Math.Min(dist1, dist2);
						if (least == dist2)
							continue;
						basesondist = true;
						
					}
					nextpost = post;
				}
			}
			
			if (nextpost)
			{
				if (basesondist)
					Decisions.Insert(nextpost);
				path.Insert( nextpost );
			}
				
			else
			{
				
				if (!Decisions.IsEmpty())
				{
					DecisionsBlacklist.Insert(Decisions[Decisions.Count() - 1]);
					Decisions.Clear();
					path.ClearPaths();
					path.Insert(PostFrom);
				}
				else
				{
					Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Path couldnt find its way to destination", basefr.GetBaseName(), baseto.GetBaseName()));
					return;
				}
			}
				
		}
		m_aPaths.Insert(path);
		
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
	static void EnableBuildingPreviews(Path path)
	{
		for (int i; i < path.Count(); i++)
		{
			LightPost PostToEnable = path.Get(i);
			if (!PostToEnable)
				continue;
			if (PostToEnable.IsBuilt())
				continue;
			PostToEnable.SetVisible();
			SCR_PathPointMapDescriptorComponent mapdesc = SCR_PathPointMapDescriptorComponent.Cast(PostToEnable.FindComponent(SCR_PathPointMapDescriptorComponent));
			mapdesc.Item().SetVisible(true);
		}
		return;
	}
	void SP_LightPostManager(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_aLightposts = {};
		m_aConnectedBases = {};
		m_instance = this;
		m_aPaths = {};
	}
	void ~SP_LightPostManager()
	{
		m_aLightposts.Clear();
		m_aConnectedBases.Clear();
		m_aPaths.Clear();
	}
}