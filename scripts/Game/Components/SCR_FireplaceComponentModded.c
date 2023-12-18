modded class SCR_FireplaceComponent
{
	[Attribute(defvalue : "2000.000")]
	protected float m_fShouldBeAliveFor;
	
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_RawMeatPrefab;
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_CookedMeatPrefab;
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_BurnedMeatPrefab;
	
	PotSlotComponent potslot;
	
	bool potatatched = false;
	
	float alivetime;
	void OnPotAtatched()
	{
		potatatched = true;
	}
	void OnPotDetatched()
	{
		potatatched = false;
	}
	override void OnPostInit(IEntity owner)
	{
		BaseSlotComponent decalComponent = BaseSlotComponent.Cast(GetOwner().FindComponent(BaseSlotComponent));
		if (decalComponent)
			m_DecalEntity = decalComponent.GetAttachedEntity();
		
		m_ComponentData = SCR_BaseInteractiveLightComponentClass.Cast(GetComponentData(GetOwner()));
		if (m_ComponentData)
			m_fCurLV = m_ComponentData.GetLightLV();
		
		if (m_DecalEntity)
			m_DecalEntity.ClearFlags(EntityFlags.VISIBLE, false);

		super.OnPostInit(owner);
		array<Managed> outComponents = {};
		owner.FindComponents(PotSlotComponent, outComponents);
		foreach (Managed slot : outComponents)
		{
			PotSlotComponent newslot = PotSlotComponent.Cast(slot);
			potslot = newslot;
		}
	}
	override void TurnOn()
	{
		alivetime = m_fShouldBeAliveFor;
		super.TurnOn();
	}
	
	override void EOnVisible(IEntity owner, int frameNumber)
	{
		super.EOnVisible(owner, frameNumber);
		
		float timeSlice = GetOwner().GetWorld().GetTimeSlice();
		
		alivetime -= timeSlice;

		if(potatatched)
		{
			IEntity Cookingpot = potslot.GetAttachedEntity();
			PrefabResource_Predicate Rawpred = new PrefabResource_Predicate(m_RawMeatPrefab);
			PrefabResource_Predicate Cookedpred = new PrefabResource_Predicate(m_CookedMeatPrefab);
			array<IEntity> RawMeat = new array<IEntity>;
			array<IEntity> CookedMeat = new array<IEntity>;
			SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(Cookingpot.FindComponent(SCR_InventoryStorageManagerComponent));
			BaseInventoryStorageComponent pInvComp = BaseInventoryStorageComponent.Cast(Cookingpot.FindComponent(BaseInventoryStorageComponent));
			inv.FindItems(RawMeat, Rawpred);
			inv.FindItems(CookedMeat, Cookedpred);
			if(CookedMeat.Count() > 0 || RawMeat.Count() > 0)
			{
				foreach(IEntity MeatPiece : CookedMeat)
				{
					FoodEntity food = FoodEntity.Cast(MeatPiece);
					bool cooked = food.Cook(timeSlice);
					if (cooked == true)
					{
						RplComponent.Cast(MeatPiece.FindComponent(RplComponent)).DeleteRplEntity(MeatPiece, true);
						inv.TrySpawnPrefabToStorage(m_BurnedMeatPrefab, pInvComp);
					}
				}
				
				foreach(IEntity MeatPiece : RawMeat)
				{
					FoodEntity food = FoodEntity.Cast(MeatPiece);
					bool cooked = food.Cook(timeSlice);
					if (cooked == true)
					{
						RplComponent.Cast(MeatPiece.FindComponent(RplComponent)).DeleteRplEntity(MeatPiece, true);
						inv.TrySpawnPrefabToStorage(m_CookedMeatPrefab, pInvComp);
					}
				}
			}
		}
		TimeAndWeatherManagerEntity TnWman = ChimeraWorld.CastFrom(GetGame().GetWorld()).GetTimeAndWeatherManager();
		ref LocalWeatherSituation Weather = new LocalWeatherSituation();
		TnWman.TryGetCompleteLocalWeather(Weather, 0, owner.GetOrigin());
		float rainint = Weather.GetRainIntensity();
		if (rainint > 0.05)
		{
			alivetime = alivetime -100;
		}
		if(alivetime <= 0)
		{
			ToggleLight(false);
		}		 		  
	}
}
class PotSlotComponentClass: BaseSlotComponentClass
{
}

class PotSlotComponent: BaseSlotComponent
{
}