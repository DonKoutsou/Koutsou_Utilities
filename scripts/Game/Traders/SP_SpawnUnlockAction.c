class SP_SpawnPointUnlock : ScriptedUserAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_PrefabToSpawn;
	
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_SpawnAmount;
	
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	
	bool Unlocked;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		IEntity Spawnpoint = GetGame().GetWorld().FindEntityByName("Spawn_Point_FIA_Initial");
		if 	(Spawnpoint)
		{
			RplComponent rpl = RplComponent.Cast(Spawnpoint.FindComponent(RplComponent));
			rpl.DeleteRplEntity(Spawnpoint, true);
		}
		vector mat[4];
		pOwnerEntity.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		Resource res = Resource.Load(m_PrefabToSpawn);
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos;
		if (res)
		{	
			for (int j = 0; j < m_SpawnAmount; j++)	
			{
				GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);
			}	
				
		}
		if (Unlocked == false)
		{
			Unlocked = true;
		}
		
	}
	override bool CanBePerformedScript(IEntity user)
	{
		if (Unlocked == true)
		{
			m_sCannotPerformReason = "Already Unlocked";
			return false;
		}
		return true;
	}
};