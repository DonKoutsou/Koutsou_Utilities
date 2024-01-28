[WorkbenchPluginAttribute(name: "Path Name Spreading World Editor Plugin",  shortcut: "Ctrl+T",  wbModules: {"WorldEditor"})]
class PathNameSpreadingWorldEditorPlugin : WorldEditorPlugin
{
	override void Run()
	{
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);

		// Get World Editor API
		WorldEditorAPI api = worldEditor.GetApi();
		
		
		api.BeginEntityAction("Path Processing entity");
		int count = api.GetSelectedEntitiesCount();
		for (int i; i < count; i++)
		{
			LightPost post = LightPost.Cast(api.GetSelectedEntity(i));
			if (!post)
				continue;
			array <LightPost> ConnectedPost = {};
			SP_LightPostManager.GetConnectedPosts(post, ConnectedPost);
			if (!ConnectedPost.IsEmpty())
			{
				array <int> ConnectingBases = {};			
				ConnectingBases.Insert(0);
				foreach(LightPost Dispost : ConnectedPost)
				{
					if (!Dispost)
						continue;
					if (Dispost.m_bRegisterInBase)
					{
						if (!ConnectingBases.Contains(Dispost.m_BaseName))
							ConnectingBases.Insert(Dispost.m_BaseName);
					}
					array <int> aConnectingBases = {};
					//Dispost.GetConnectingBasesi(aConnectingBases);					
					foreach(int base: aConnectingBases)
					{
						if (!ConnectingBases.Contains(base))
							ConnectingBases.Insert(base);
					}
				}
				IEntitySource source = api.EntityToSource(post);
				Print(ConnectingBases.ToString().Length());
				//Print(ConnectingBases.ToString().Substring(18, 126));
				string thing = "{ ";
				for (int g; g < ConnectingBases.Count(); g++)
				{
					thing = thing + ConnectingBases[g];
					if (g + 1 < ConnectingBases.Count())
					{
						thing = thing + ", ";
					}
				}
				thing = thing + "}";
				Print(thing);
				Print(thing.Length());
				api.SetVariableValue(source, {}, "m_aConnectingBases", thing);
			}
		}
		api.EndEntityAction();
		api.BeginEntityAction("Path Processing entity");
		for (int i = count -1; i >= 0; i--)
		{
			LightPost post = LightPost.Cast(api.GetSelectedEntity(i));
			if (!post)
				continue;
			array <LightPost> ConnectedPost = {};
			SP_LightPostManager.GetConnectedPosts(post, ConnectedPost);
			if (!ConnectedPost.IsEmpty())
			{
				array <int> ConnectingBases = {};			
				ConnectingBases.Insert(0);
				foreach(LightPost Dispost : ConnectedPost)
				{
					if (!Dispost)
						continue;
					if (Dispost.m_bRegisterInBase)
					{
						if (!ConnectingBases.Contains(Dispost.m_BaseName))
							ConnectingBases.Insert(Dispost.m_BaseName);
					}
					array <int> aConnectingBases = {};
					//Dispost.GetConnectingBasesi(aConnectingBases);					
					foreach(int base: aConnectingBases)
					{
						if (!ConnectingBases.Contains(base))
							ConnectingBases.Insert(base);
					}
				}
				IEntitySource source = api.EntityToSource(post);
				string thing = "{ ";
				for (int g; g < ConnectingBases.Count(); g++)
				{
					thing = thing + ConnectingBases[g];
					if (g + 1 < ConnectingBases.Count())
					{
						thing = thing + ", ";
					}
				}
				thing = thing + "}";

				api.SetVariableValue(source, {}, "m_aConnectingBases", thing);
			}
		}
		api.EndEntityAction();
	}
}
[WorkbenchPluginAttribute(name: "Path Generation World Editor Plugin",  wbModules: {"WorldEditor"})]
class PathGenerationWorldEditorPlugin : WorldEditorPlugin
{
	ref array <Path> m_aPaths = {};
	override void Run()
	{
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);

		// Get World Editor API
		WorldEditorAPI api = worldEditor.GetApi();
		
		api.BeginEntityAction("Path Processing entity");
		int count = api.GetSelectedEntitiesCount();
		for (int i = 1; i < count; i++)
		{
			LightPost post = LightPost.Cast(api.GetSelectedEntity(i));
			if (!post)
				continue;
			
			
			for (int v = 1; v < count; v++)
			{
				if (i == v)
					continue;
				LightPost post2 = LightPost.Cast(api.GetSelectedEntity(v));
				if (!post2)
					continue;
				
				CreatePath(post, post2);
			}
		}
		IEntitySource source = api.EntityToSource(GetGame().GetGameMode());
		api.ModifyComponentKey(GetGame().GetGameMode(), "SP_LightPostManager", "m_aPaths", m_aPaths.ToString());
		//api.SetVariableValue(source, {}, "m_aConnectingBases", ConnectingBases.ToString());
		api.EndEntityAction();
	}
	void CreatePath(LightPost PostFrom, LightPost PostTo)
	{
			SP_BaseEn BaseFrom = PostFrom.m_BaseName;
			SP_BaseEn BaseTo = PostTo.m_BaseName;	
	
			Path path = new Path();
			path.AddBase(SP_BaseNames.Get(BaseFrom));
			path.AddBase(SP_BaseNames.Get(BaseTo));
		
		
			path.Insert(PostFrom);
			
			
			array <LightPost> Decisions = {};
			array <LightPost> DecisionsBlacklist = {};
			while (!path.Contains(PostTo))
			{
				LightPost currentpost = path.Get(path.Count() - 1);
				
				LightPost nextpost;
				array <LightPost> ConnectedPosts = {};
				SP_LightPostManager.GetConnectedPosts(currentpost, ConnectedPosts);
				bool basesondist;
				foreach (LightPost post : ConnectedPosts)
				{
					if (path.Contains(post))
						continue;
					if (DecisionsBlacklist.Contains(post))
						continue;
					//if (post.m_aConnectingBases.Contains(BaseTo) || post.m_iPathDirections > 2)
					//{
						if (nextpost)
						{
							float dist1 = vector.Distance(post.GetOrigin(), PostTo.GetOrigin());
							float dist2 = vector.Distance(nextpost.GetOrigin(), PostTo.GetOrigin());
							float least = Math.Min(dist1, dist2);
							if (least == dist2)
								continue;
							basesondist = true;
							
						}
						nextpost = post;
					//}
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
						//Print(string.Format("PATH CREATION FAILED: creation of path from %1 to %2 was unsuccesfull. Path couldnt find its way to destination", basefr.GetBaseName(), baseto.GetBaseName()));
						return;
					}
				}
					
			}
			m_aPaths.Insert(path);
			
	}
}