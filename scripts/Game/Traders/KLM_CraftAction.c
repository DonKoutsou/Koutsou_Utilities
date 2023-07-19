//------------------------------------------------------------------------------------------------

class KLM_CraftAction : KLM_BaseTraderAction
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_CraftedObject;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_CraftedNumber;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;	
	[Attribute("false", UIWidgets.CheckBox, params: "", desc: "Should remove parent object after action")]
	bool m_bShouldRemoveParent;
	
	override bool Sell(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		vector mat[4];
		pOwnerEntity.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		
		Resource res = Resource.Load(m_CraftedObject);
		if (res)
		{	
			for (int j = 0; j < m_CraftedNumber; j++)		
				GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);
			
			if (m_bShouldRemoveParent && !HasUses())
				delete pOwnerEntity;
			
			return true;
		}
		return false;
	}
}
