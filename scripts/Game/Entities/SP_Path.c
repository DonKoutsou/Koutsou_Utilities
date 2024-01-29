class Path
{
	ref array <LightPost> m_posts = {};
	ref array <string> m_bases = {};
	
	void AddBase(string base)
	{
		m_bases.Insert(base);
	}
	void GetBases(array <string> PathBases)
	{
		PathBases.Copy(m_bases);
	}
	
	void Insert(LightPost post)
	{
		m_posts.Insert(post);
	}
	void Remove(int i)
	{
		m_posts.Remove(i);
	}
	void ClearPaths()
	{
		m_posts.Clear();
	}
	bool ConnectsBases(string form, string to)
	{
		return (m_bases.Contains(form) && m_bases.Contains(to));
	}
	bool IsForBase(string base)
	{
		return m_bases.Contains(base);
	}
	void SpawnTaskMarkers(IEntity Assignee)
	{
		if (m_posts.IsEmpty())
			return;
		foreach (LightPost post : m_posts)
		{
			post.SpawnTaskMarkers(Assignee);
		}
	}
	LightPost GetPost(int i)
	{
		if (i >= m_posts.Count())
			return null;
		return m_posts[i];
	}
	void GetPosts(out array <LightPost> posts)
	{
		posts.Copy(m_posts);
	}
	int Count()
		return m_posts.Count();
	LightPost Get(int index)
		return m_posts[index];
	bool Contains(LightPost post)
		return m_posts.Contains(post);
	
	void Path();
	void ~Path();
}
class SP_ColorNames
{
	static Color Get(SP_EColor en)
	{
		if (en == SP_EColor.RED)
			return Color.Red;
		if (en == SP_EColor.BLUE)
			return Color.Blue;
		if (en == SP_EColor.YELLOW)
			return Color.Yellow;
		if (en == SP_EColor.GREEN)
			return Color.Green;
		if (en == SP_EColor.VIOLET)
			return Color.Violet;
		return Color.White;
	}
}
enum SP_EColor
{
	RED,
	BLUE,
	YELLOW,
	GREEN,
	VIOLET,
}