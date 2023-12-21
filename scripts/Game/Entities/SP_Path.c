class Path
{
	ref array <LightPost> posts = {};
	ref array <string> bases = {};
	
	void AddBase(string base)
	{
		bases.Insert(base);
	}
	void GetBases(array <string> PathBases)
	{
		PathBases.Copy(bases);
	}
	
	void Insert(LightPost post)
	{
		posts.Insert(post);
	}
	
	int Count()
		return posts.Count();
	LightPost Get(int index)
		return posts[index];
	bool Contains(LightPost post)
		return posts.Contains(post);
	
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