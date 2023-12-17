class LightPostClass: GameEntityClass{};
class LightPost: GameEntity
{
	[Attribute()]
	ref array <string> m_aConnectingBases;
	
	bool m_bBuilt;
	
	bool IsBuilt()
	{
		return m_bBuilt;
	}
	void SetBuilt(bool built)
	{
		m_bBuilt = built;
	}
	bool ConnectBases(array <string> bases)
	{
		foreach (string Basename : bases)
		{
			if (!m_aConnectingBases.Contains( Basename ) )
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
		SP_LightPostManager.RegisterPost(this);
		//GetGame().GetCallqueue().CallLater(SetVisible, 2000, false);
	};
	//Destructor
	void ~LightPost()
	{
		SP_LightPostManager.UnRegisterPost(this);
	};
}
class SP_LightPostManagerClass : ScriptComponentClass
{
	
};
[BaseContainerProps(configRoot: true)]
class SP_LightPostManager : ScriptComponent
{

	static ref array <LightPost> m_aLightposts = {};
	
	static void RegisterPost(LightPost Post)
	{
		if (!m_aLightposts.Contains(Post))
			m_aLightposts.Insert(Post);
	}
	static void UnRegisterPost(LightPost Post)
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

		}
		return;
	}
}