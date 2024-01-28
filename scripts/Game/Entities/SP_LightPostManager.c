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
	static void GetPath(out array <Path> paths, string BaseFrom, string BaseTo)
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
		//array <string> bases = {};
		//post.GetConnectingBases(bases);
		//if (AreBasesConnected(bases))
		//{
		//	OnBasesConnected(bases);
		//}
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

	void GeneratePaths()
	{
		RegisterPostsToBases();
		baseman = SCR_GameModeCampaign.Cast(GetGame().GetGameMode()).GetBaseManager();
		SetEventMask(GetOwner(), EntityEvent.FIXEDFRAME);
	}
	
	int index1 = 1, index2 = 0;
	int max = 46;
	SCR_CampaignMilitaryBaseManager baseman;
	
	ref array <LightPost> Decisions = {};
	ref array <LightPost> DecisionsBlacklist = {};
	
	SCR_CampaignMilitaryBaseComponent m_basefr;
	SCR_CampaignMilitaryBaseComponent m_baseto;
	
	LightPost PostFrom;
	LightPost PostTo;
	
	bool creatingpath;
	
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if (creatingpath)
		{
			TryToFinishPath();
			return;
		}
		index2 += 1;
		if (index2 > max)
		{
			index2 = 1;
			index1 += 1;
			if (index1 > max)
			{
				ClearEventMask(owner, EntityEvent.FIXEDFRAME);
				return;
			}
		}
		SP_BaseEn BaseFrom = index1;
		m_basefr = baseman.GetNamedBase(SP_BaseNames.Get( BaseFrom ));
		if (!m_basefr)
		{
			Print(string.Format("PATH CREATION FAILED: creation of path from %1 was unsuccesfull. Reason : %1 doesent exist", SP_BaseNames.Get( BaseFrom )));
			index1 += 1;
			return;
		}
		SP_BaseEn BaseTo = index2;
		m_baseto = baseman.GetNamedBase(SP_BaseNames.Get( BaseTo ));
		if (!m_baseto)
		{
			Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Reason : %2 doesent exist", SP_BaseNames.Get( BaseFrom ), SP_BaseNames.Get( BaseTo )));
			index2 += 1;
			return;
		}
		CreatePath(m_basefr, m_baseto);
		
	}
	void PrintPaths(float timeslice)
	{
		cooldown = cooldown - timeslice;
		if (m_aPaths.IsEmpty())
			return;
		//if (pathnum >= m_aPaths.Count())
			//pathnum = m_aPaths.Count() - 1;

		pathnum = m_aPaths.Count() - 1;
		
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
			//pathnum ++;
			Path pathtoprint2 = m_aPaths[pathnum];
			Print(string.Format("Printing path from %1 to %2", SCR_StringHelper.Translate( pathtoprint2.m_bases[0] ), SCR_StringHelper.Translate( pathtoprint2.m_bases[1])));
		}
	}
	void TryToFinishPath()
	{
			//Print(string.Format("Trying to finish path from %1 to %2", SCR_StringHelper.Translate( m_basefr.GetBaseName() ), SCR_StringHelper.Translate( m_baseto.GetBaseName())));
			Path curpath = m_aPaths[m_aPaths.Count() - 1];
			LightPost currentpost = curpath.GetPost(curpath.Count() - 1);
			
			LightPost nextpost;
			array <LightPost> ConnectedPosts = {};
			GetConnectedPosts(currentpost, ConnectedPosts);
			bool basesondist;
			if (ConnectedPosts.Count() > 2)
						basesondist = true;
			foreach (LightPost post : ConnectedPosts)
			{
				if (curpath.Contains(post))
					continue;
				if (DecisionsBlacklist.Contains(post))
					continue;
				//if (post.m_aConnectingBases.Contains(SP_BaseNames.Convert(m_baseto)))
				//{
					
				if (nextpost)
				{
					float dist1 = vector.Distance(post.GetOrigin(), m_baseto.GetOwner().GetOrigin());
					float dist2 = vector.Distance(nextpost.GetOrigin(), m_baseto.GetOwner().GetOrigin());
					float least = Math.Min(dist1, dist2);
					if (least == dist2)
						continue;
				}
				nextpost = post;
					
				//}
			}
			
			if (nextpost)
			{
				if (nextpost.m_bRegisterInBase)
				{
					bool exists;
					for (int i; i < m_aPaths.Count(); i++)
					{
						if (m_aPaths[i].m_bases.Contains(m_basefr.GetBaseName()) && m_aPaths[i].m_bases.Contains(SP_BaseNames.Get( nextpost.m_BaseName) ))
						{
							exists = true;
						}
					}
					if (!exists)
					{
						Path path = new Path();
						path.AddBase(m_basefr.GetBaseName());
						path.AddBase(SP_BaseNames.Get( nextpost.m_BaseName) );
						path.m_posts.Copy(curpath.m_posts);
						m_aPaths.InsertAt(path, m_aPaths.Count() - 1);
						Print(string.Format("ACCIDENTAL PATH CREATION : A path from %1 to %2 was accidently created, added to list of paths.", SCR_StringHelper.Translate( m_basefr.GetBaseName() ), SCR_StringHelper.Translate( SP_BaseNames.Get( nextpost.m_BaseName))));
					}
				}
				if (basesondist)
					Decisions.Insert(nextpost);
				curpath.Insert( nextpost );
			}
				
			else
			{
				
				if (!Decisions.IsEmpty())
				{
					Print(string.Format("PATH CREATION RESTARTING: creation of path from %1 to %2 was couldnt progress further.", SCR_StringHelper.Translate( m_basefr.GetBaseName() ), SCR_StringHelper.Translate( m_baseto.GetBaseName())));
					DecisionsBlacklist.Insert(Decisions[Decisions.Count() - 1]);
					while (curpath.GetPost(curpath.Count() - 1) != Decisions[Decisions.Count() - 1])
					{
						curpath.Remove(curpath.Count() - 1);
					};
					curpath.Remove(curpath.Count() - 1);
					Decisions.Remove(Decisions.Count() - 1);
					//curpath.ClearPaths();
					//curpath.Insert(PostFrom);
				}
				else
				{
					Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Path couldnt find its way to destination", SCR_StringHelper.Translate( m_basefr.GetBaseName() ), SCR_StringHelper.Translate( m_baseto.GetBaseName())));
					ProgressToNextPath();
					return;
				}
			}
			if (curpath.Contains(PostTo))
			{
				Print(string.Format("PATH CREATION FINISHED: creation of path from %1 to %2 was succesfull.", SCR_StringHelper.Translate( m_basefr.GetBaseName() ), SCR_StringHelper.Translate( m_baseto.GetBaseName())));
				ProgressToNextPath();
			}
	}
	void ProgressToNextPath()
	{
		Decisions.Clear();
		DecisionsBlacklist.Clear();
		
		m_basefr = null;;
		m_baseto = null;;
		
		PostFrom = null;
		PostTo = null;
		
		creatingpath = false;
	}
	void CreatePath(SCR_CampaignMilitaryBaseComponent basefr, SCR_CampaignMilitaryBaseComponent baseto)
	{
		for (int i; i < m_aPaths.Count(); i++)
		{
			if (m_aPaths[i].m_bases.Contains(basefr.GetBaseName()) && m_aPaths[i].m_bases.Contains(baseto.GetBaseName()))
			{
				Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Reason : Path already exists", basefr.GetBaseName(), baseto.GetBaseName()));
				return;
			}
		}
		PostFrom = basefr.GetBasePost();
		PostTo = baseto.GetBasePost();

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

		
		m_aPaths.Insert(path);
		creatingpath = true;
		Print(string.Format("PATH CREATION STARTING: Starting to create path from %1 to %2",  SCR_StringHelper.Translate( m_basefr.GetBaseName() ), SCR_StringHelper.Translate( m_baseto.GetBaseName())));
	}
	static bool AreBasesConnected(array <string> bases)
	{
		array <Path> Paths = {};
		array <LightPost> Posts = {};
		SP_LightPostManager.GetPath(Paths, bases[0], bases[1]);
		Paths[0].GetPosts(Posts);
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