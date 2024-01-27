[WorkbenchPluginAttribute(name: "Path Generation World Editor Plugin",  shortcut: "Ctrl+T",  wbModules: {"WorldEditor"})]
class PathGenerationWorldEditorPlugin : WorldEditorPlugin
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
				array <SP_BaseEn> ConnectingBases = {};			
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
					foreach(int base: Dispost.m_aConnectingBases)
					{
						if (!ConnectingBases.Contains(base))
							ConnectingBases.Insert(base);
					}
				}
				IEntitySource source = api.EntityToSource(post);
				api.SetVariableValue(source, {}, "m_aConnectingBases", ConnectingBases.ToString());
			}
		}
		for (int i = count -1; i >= 0; i--)
		{
			LightPost post = LightPost.Cast(api.GetSelectedEntity(i));
			if (!post)
				continue;
			array <LightPost> ConnectedPost = {};
			SP_LightPostManager.GetConnectedPosts(post, ConnectedPost);
			if (!ConnectedPost.IsEmpty())
			{
				array <SP_BaseEn> ConnectingBases = {};			
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
					foreach(int base: Dispost.m_aConnectingBases)
					{
						if (!ConnectingBases.Contains(base))
							ConnectingBases.Insert(base);
					}
				}
				IEntitySource source = api.EntityToSource(post);
				api.SetVariableValue(source, {}, "m_aConnectingBases", ConnectingBases.ToString());
			}
		}
		api.EndEntityAction();
	}
}