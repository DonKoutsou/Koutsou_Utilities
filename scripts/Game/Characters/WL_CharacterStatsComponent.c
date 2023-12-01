#ifdef WORKBENCH
	#define WL_CHARACTER_STATS_DEBUG
#endif
class SP_ClothAttributes: BaseItemAttributeData
{
	[Attribute("0.3")]
	private float m_TemperatureIsolation;
	
	//------------------------------------------------------------------------------------------------	
	float GetTemperatureIsolation() 
	{
		return m_TemperatureIsolation;
	}
};
[BaseContainerProps()]
class SP_CharacterHasEatableItemEquippedCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	//! Returns true when current controlled entity has specified gadget
	//! Returns opposite if m_bNegateCondition is enabled
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
		if (!data)
			return false;

		IEntity item = data.GetCurrentItemEntity();
		if (!item)
			return GetReturnResult(false);

		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		bool result = consumable && consumable.IsEatable();

		return GetReturnResult(result);
	}
};

[BaseContainerProps()]
class SP_ConsumableEffectEat : SCR_ConsumableEffectHealthItems
{
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
	
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, SCR_ConsumableEffectAnimationParameters animParams)
	{
		super.ApplyEffect(target, user, item, animParams);
		PlayerController playerController = GetGame().GetPlayerController();
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		if (!statComponent)
			return;
		
		statComponent.Eat(item);
	}
}
[BaseContainerProps()]
class SP_ConsumableEffectPills : SCR_ConsumableEffectHealthItems
{
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
	
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, SCR_ConsumableEffectAnimationParameters animParams)
	{
		super.ApplyEffect(target, user, item, animParams);
		PlayerController playerController = GetGame().GetPlayerController();
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		if (!statComponent)
			return;
		
		statComponent.TireDown(item);
	}
}

[BaseContainerProps()]
class SP_CharacterHasDrinkableItemEquippedCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	//! Returns true when current controlled entity has specified gadget
	//! Returns opposite if m_bNegateCondition is enabled
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
		if (!data)
			return false;

		IEntity item = data.GetCurrentItemEntity();
		if (!item)
			return GetReturnResult(false);

		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		bool result = consumable && consumable.IsDrinkable() && consumable.HasUses();

		return GetReturnResult(result);
	}
};

[BaseContainerProps()]
class SP_ConsumableEffectDrink : SCR_ConsumableEffectHealthItems
{
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, SCR_ConsumableEffectAnimationParameters animParams)
	{
		super.ApplyEffect(target, user, item, animParams);
		PlayerController playerController = GetGame().GetPlayerController();
		SP_CharacterStatsComponent statComponent = SP_CharacterStatsComponent.Cast(playerController.FindComponent(SP_CharacterStatsComponent));
		if (!statComponent)
			return;
		
		statComponent.Drink(item);
	}
}

[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class SP_CharacterStatsComponentClass : ScriptComponentClass
{
	[Attribute("0.01", params: "0.0 inf", desc: "How much hunger should changed per second", category: "Hunger")]
	float m_fHungerChangedPerSecond;
	
	[Attribute("0.02", params: "0.0 inf", desc: "How much thirst should changed per second", category: "Thirst")]
	float m_fThirstChangedPerSecond;
	
	[Attribute("0.02", params: "0.0 inf", desc: "How much thirst should changed per second", category: "Energy")]
	float m_fEnergyChangedPerSecond;
	
	[Attribute("5", params: "0.0 inf", desc: "Time between each tick of stats on server [s]")]
	float m_fTimeBetweenEachTick;
};

class SP_CharacterStatsComponent : ScriptComponent
{
	[RplProp(onRplName: "OnHungerChanged", condition: RplCondition.OwnerOnly)]
	protected float m_fHunger = 100.0;
	[RplProp(onRplName: "OnThirstChanged", condition: RplCondition.OwnerOnly)]
	protected float m_fThirst = 100.0;
	[RplProp(onRplName: "OnEnergyChanged", condition: RplCondition.OwnerOnly)]
	protected float m_fEnergy = 100.0;
	
	
	float MAX_CLOTHES_FACTOR = 1.0;
	float FIREPLACE_RANGE = 3;
	
	protected float m_fTemperature;
	
		float clothesFactor = 0;
	// update temperature based on clothes, weather and possibly other factors
	
	[Attribute("42.0")]
	float m_fMaxTemperature;	
	
	[Attribute("36.6")]
	float m_fHealthyTemperature;
	
	[Attribute("35.0")]
	float m_fColdnessTemperature;
	
	[Attribute("32.0")]
	float m_fUnconsciousTemperature;
	
	[Attribute("29.0")]
	float m_fDeathTemperature;
	
	[Attribute("39.0")]
	float m_fThirstTemperature;
	
	[Attribute("20")]
	float m_fUnconEnergy;
	
	// weather manager
	TimeAndWeatherManagerEntity m_pWeather;
	// character owner
	ChimeraCharacter m_pChar;
	// damage manager
	SCR_DamageManagerComponent m_pDamage;
	// loadout manager
	EquipedLoadoutStorageComponent m_pLoadout;
	// maybe stamina
	
	float m_FireplacesHeat = 0;
	protected float m_fTime;
	protected float p_Drain;
	protected RplComponent m_pRplComponent;
	protected EventHandlerManagerComponent m_pEventHandlerManager;
	bool MissingSomething(out int reasonindex)
	{
		if (GetCurrentTemp() < m_fHealthyTemperature - 5)
		{
			reasonindex = 0;
			return true;
		}
		if (GetCurrentTemp() > m_fHealthyTemperature + 5)
		{
			reasonindex = 1;
			return true;
		}
		if (GetHunger() < 20)
		{
			reasonindex = 2;
			return true;
		}
		if (GetThirst() < 20)
		{
			reasonindex = 3;
			return true;
		}
		if (GetEnergy() < 20)
		{
			reasonindex = 4;
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------
	SP_CharacterStatsComponentClass GetCharacterStatsPrefabData()
	{
		return SP_CharacterStatsComponentClass.Cast(GetComponentData(GetOwner()));
	}
	
	void CheckTemperature()
	{
		if (!m_pChar.GetCharacterController().IsUnconscious())
		{
			if (m_fTemperature < m_fColdnessTemperature)
			{
				SCR_CameraShakeManagerComponent.AddCameraShake(0.2, 0.05, 0.05, 0.2, 0.5);
			}
			if (m_fTemperature < m_fUnconsciousTemperature)
			{
				m_pChar.GetCharacterController().SetUnconscious(true);
			}
		}
		if (m_fTemperature <= m_fDeathTemperature)
		{
			m_pDamage.Kill(Instigator.CreateInstigator(m_pChar));
		}
		if (m_fTemperature >= m_fMaxTemperature)
		{
			m_pDamage.Kill(Instigator.CreateInstigator(m_pChar));
		}
	}
	void CheckEnergy()
	{
		if (!m_pChar.GetCharacterController().IsUnconscious())
		{
			if (m_fEnergy < m_fUnconEnergy)
			{
				m_pChar.GetCharacterController().SetUnconscious(true);
			}
		}
		else
		{
			if (m_fEnergy > m_fUnconEnergy + 1)
			{
				m_pChar.GetCharacterController().SetUnconscious(false);
			}
		}
	}
	
	float GetNeedClothes(float outsideTemp)
	{
		// temp from 5 - 25
		// clothes from 1.0 - 0
		
	    float minTemp = 5.0;
	    float maxTemp = 25.0;
	    float minClothes = 1.0;
	    float maxClothes = 0.0;
	
	    if (outsideTemp <= minTemp)
	        return minClothes;
	    if (outsideTemp >= maxTemp)
	        return maxClothes;
	
	    return minClothes + (maxClothes - minClothes) * (outsideTemp - minTemp) / (maxTemp - minTemp);
	}
	

	bool FindFireplace(IEntity ent)
	{
		SCR_FireplaceComponent fire = SCR_FireplaceComponent.Cast(ent.FindComponent(SCR_FireplaceComponent));
		if (fire)
		{
			if (!fire.IsOn())
				return true;
			// heat should have falloff
			// at distance of 2 the heat should be 0
			// at distance 0 heat should be 20
			SCR_PlayerController cont = SCR_PlayerController.Cast(GetOwner());
			float dist = (ent.GetOrigin() - cont.GetControlledEntity().GetOrigin()).Length();
			float heat = (FIREPLACE_RANGE - dist) / FIREPLACE_RANGE * 30;
			
			if (heat > 0)
				m_FireplacesHeat = m_FireplacesHeat + heat;
			
			// early exit query if heat found is significant
			if (m_FireplacesHeat > 5)
				return false;
		}
		return true;
	}
	
	float GetClothCoverage(IEntity cloth)
	{
		InventoryItemComponent item = InventoryItemComponent.Cast(cloth.FindComponent(InventoryItemComponent));
		if (!item)
			return 0;
		
		SP_ClothAttributes at = SP_ClothAttributes.Cast(item.FindAttribute(SP_ClothAttributes));
		if (!at)
			return 0;
		
		return at.GetTemperatureIsolation();
	}
	float GetCurrentTemp()
	{
		return m_fTemperature;
	}
	float GetHunger()
	{
		return m_fHunger;
	}
	float GetThirst()
	{
		return m_fThirst;
	}
	float GetEnergy()
	{
		return m_fEnergy;
	}
	//! Only on authoritative machine
	void SetNewHunger(float newHunger)
	{
		m_fHunger = Math.Clamp(newHunger, 0.0, 100.0);
		
		if (!System.IsConsoleApp())
			OnHungerChanged();
		
		Replication.BumpMe();
	}
	
	//! Only on authoritative machine
	void SetNewThirst(float newThirst)
	{
		m_fThirst = Math.Clamp(newThirst, 0.0, 100.0);
		
		if (!System.IsConsoleApp())
			OnThirstChanged();
		
		Replication.BumpMe();
	}
	void SetNewEnergy(float newEnergy)
	{
		m_fEnergy = Math.Clamp(newEnergy, 0.0, 100.0);
		
		if (!System.IsConsoleApp())
			OnEnergyChanged();
		
		Replication.BumpMe();
	}
	
	protected void OnHungerChanged()
	{
		if (m_pEventHandlerManager)
			m_pEventHandlerManager.RaiseEvent("OnHungerChanged", 1, m_fHunger);
	}
	
	protected void OnThirstChanged()
	{
		if (m_pEventHandlerManager)
			m_pEventHandlerManager.RaiseEvent("OnThirstChanged", 1, m_fThirst);
	}
	protected void OnEnergyChanged()
	{
		if (m_pEventHandlerManager)
			m_pEventHandlerManager.RaiseEvent("OnEnergyChanged", 1, m_fEnergy);
	}
	protected void OnTempChange()
	{
		if (m_pEventHandlerManager)
			m_pEventHandlerManager.RaiseEvent("OnTempChange", 1, m_fTemperature);
	}
	//! Only on authoritative machine
	void Eat(IEntity item)
	{
		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if(consumable.HasUses() == false)
		{
			return;
		}
		if (m_pRplComponent && m_pRplComponent.IsProxy())
		{
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!itemComponent)
				return;
			
			RplId itemComponentRplID = Replication.FindId(itemComponent);
			Rpc(RpcAsk_Eat, itemComponentRplID);
			return;
		}
		if(m_fHunger + consumable.GetUseAmount() > 100.0)
		{
			float newuseamount = 100 - m_fHunger;
			SetNewHunger(m_fHunger + newuseamount);
			consumable.Use(newuseamount);
		}
		else
		{
			SetNewHunger(m_fHunger + consumable.GetUseAmount());
			consumable.Use(consumable.GetUseAmount());
		}
		if(consumable.HasUses() == false && consumable.GetConsumableEffect().GetDeleteOnUse() == true)
			RplComponent.DeleteRplEntity(item, false);
	}
	void TireDown(IEntity item)
	{
		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if(consumable.HasUses() == false)
		{
			return;
		}
		if (m_pRplComponent && m_pRplComponent.IsProxy())
		{
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!itemComponent)
				return;
			
			RplId itemComponentRplID = Replication.FindId(itemComponent);
			Rpc(RpcAsk_TireDown, itemComponentRplID);
			return;
		}
		SetNewEnergy(m_fEnergy + -consumable.GetUseAmount());
		consumable.Use(consumable.GetUseAmount());
		if(consumable.HasUses() == false && consumable.GetConsumableEffect().GetDeleteOnUse() == true)
			RplComponent.DeleteRplEntity(item, false);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Eat(RplId itemRplID)
	{
		if (!itemRplID.IsValid())
			return;
		
		Managed rplItem = Replication.FindItem(itemRplID);
		if (!rplItem)
			return;
		
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(rplItem);
		if (!itemComp)
			return;
		
		IEntity item = itemComp.GetOwner();
		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if (consumable && consumable.IsEatable())
			Eat(item);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_TireDown(RplId itemRplID)
	{
		if (!itemRplID.IsValid())
			return;
		
		Managed rplItem = Replication.FindItem(itemRplID);
		if (!rplItem)
			return;
		
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(rplItem);
		if (!itemComp)
			return;
		
		IEntity item = itemComp.GetOwner();
		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if (consumable && consumable.IsEatable())
			TireDown(item);
	}
	
	//! Only on authoritative machine
	void Drink(IEntity item)
	{
		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if(consumable.HasUses() == false)
		{
			return;
		}
		if (m_pRplComponent && m_pRplComponent.IsProxy())
		{
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!itemComponent)
				return;
			
			RplId itemComponentRplID = Replication.FindId(itemComponent);
			Rpc(RpcAsk_Drink, itemComponentRplID);
			return;
		}
		if(m_fThirst + consumable.GetUseAmount() > 100.0)
		{
			float newuseamount = 100 - m_fThirst;
			SetNewThirst(m_fThirst + newuseamount);
			consumable.Use(newuseamount);
			CoolDown(newuseamount); 
		}
		else
		{
			SetNewThirst(m_fThirst + consumable.GetUseAmount());
			consumable.Use(consumable.GetUseAmount());
			CoolDown(consumable.GetUseAmount()); 
		}
		if(consumable.HasUses() == false && consumable.GetConsumableEffect().GetDeleteOnUse() == true)
			RplComponent.DeleteRplEntity(item, false);
	}
	void CoolDown(float useamount)
	{
		float temp = GetCurrentTemp();
		if (temp > 39)
		{
			m_fTemperature = GetCurrentTemp() - useamount/100;
		}
	}
	void Sleep(IEntity itemm, int amount)
	{
		SetNewEnergy(m_fEnergy + amount);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Drink(RplId itemRplID)
	{
		if (!itemRplID.IsValid())
			return;
		
		Managed rplItem = Replication.FindItem(itemRplID);
		if (!rplItem)
			return;
		
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(rplItem);
		if (!itemComp)
			return;
		
		IEntity item = itemComp.GetOwner();
		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if (consumable && consumable.IsDrinkable())
			Drink(item);
	}
	
	//! Only on authoritative machine
	void OnDamageStateChanged(EDamageState state)
	{
		if (m_pRplComponent && m_pRplComponent.IsProxy())
			return;
		
		if (state == EDamageState.DESTROYED)
			ClearEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fTime += timeSlice;
		
		float timeBtwEachTick = GetCharacterStatsPrefabData().m_fTimeBetweenEachTick;
		if (m_fTime >= timeBtwEachTick && m_pLoadout)
		{
			clothesFactor = 0;
				// jacket
			IEntity jacket = m_pLoadout.GetClothFromArea(LoadoutJacketArea);
			if (jacket)
			{
				clothesFactor = clothesFactor + GetClothCoverage(jacket);
			}
			// pants
			IEntity pants = m_pLoadout.GetClothFromArea(LoadoutPantsArea);
			if (pants)
			{
				clothesFactor = clothesFactor + GetClothCoverage(pants);
			}
			IEntity Vest = m_pLoadout.GetClothFromArea(LoadoutArmoredVestSlotArea);
			if (Vest)
			{
				clothesFactor = clothesFactor + GetClothCoverage(Vest);
			}
			// headgear
			IEntity headgear = m_pLoadout.GetClothFromArea(LoadoutHeadCoverArea);
			if (headgear)
			{
				clothesFactor = clothesFactor + GetClothCoverage(headgear);
			}
			// boots
			IEntity boots = m_pLoadout.GetClothFromArea(LoadoutBootsArea);
			if (boots)
			{
				clothesFactor = clothesFactor + GetClothCoverage(boots);
			}
			if (m_pDamage.IsDestroyed())
			return;	
		
			
			
			
			vector origin = m_pChar.GetOrigin();
			// consider fireplaces
			m_FireplacesHeat = 0;
			owner.GetWorld().QueryEntitiesBySphere(origin, FIREPLACE_RANGE, FindFireplace);
			float outsideTemperature = m_pWeather.CalculateFeelsLikeTemperature(m_pChar);
			outsideTemperature = outsideTemperature + m_FireplacesHeat;
			
			clothesFactor = Math.Clamp(clothesFactor, 0, MAX_CLOTHES_FACTOR);
			float needClothes = GetNeedClothes(outsideTemperature);
			
			// apply factors on current temperature
			const float cooldownRate = 0.1;
			
			float diffTemp = (clothesFactor - needClothes) * timeSlice;
			if (diffTemp >= 0.01 || diffTemp <= -0.01)
				m_fTemperature = Math.Clamp(m_fTemperature + diffTemp + p_Drain/10, 0, m_fMaxTemperature);
			else
				m_fTemperature = m_fTemperature + ((m_fHealthyTemperature - m_fTemperature)/ 50);
				
			Print(string.Format("temp %1 / %2 diff %3 cloth %4 / %5", m_fTemperature, outsideTemperature, diffTemp, clothesFactor, needClothes));
			
			// check temperature for crossing thresholds and effects
			CheckTemperature();
			CheckEnergy();
			OnTempChange();
			OnEnergyChanged();
			float hungerloss = GetCharacterStatsPrefabData().m_fHungerChangedPerSecond + p_Drain;
			float thirstloss;
			if (m_fTemperature > m_fThirstTemperature)
				thirstloss = GetCharacterStatsPrefabData().m_fThirstChangedPerSecond * 2 + p_Drain;
			else
				thirstloss = GetCharacterStatsPrefabData().m_fThirstChangedPerSecond + p_Drain;
			float energyloss = GetCharacterStatsPrefabData().m_fEnergyChangedPerSecond + p_Drain;
			SetNewHunger(m_fHunger - hungerloss * timeBtwEachTick);
			SetNewThirst(m_fThirst - thirstloss * timeBtwEachTick);
			
			
			SCR_CharacterDamageManagerComponent dmgman = 	SCR_CharacterDamageManagerComponent.Cast(m_pChar.FindComponent(SCR_CharacterDamageManagerComponent));
			if (dmgman)
			{
				if (!dmgman.GetIsUnconscious())
					SetNewEnergy(m_fEnergy - energyloss * timeBtwEachTick);
				else
					SetNewEnergy(m_fEnergy + energyloss * timeBtwEachTick);
			}
			
				
			//Print("hunger drain =" + hungerloss);
			//Print("thirst drain =" + thirstloss);
			p_Drain = 0;
			m_fTime -= timeBtwEachTick;
			
			if (m_fHunger <= 0 || m_fThirst <= 0)
			{
				if (m_pDamage && !m_pDamage.IsDestroyed())
				{
					m_pDamage.Kill(Instigator.CreateInstigator(m_pChar));
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		SCR_PlayerController cont = SCR_PlayerController.Cast(owner);
		if (!cont)
			return;
		cont.m_OnControlledEntityChanged.Insert(Init);
		SCR_RespawnSystemComponent respawn = SCR_RespawnSystemComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_RespawnSystemComponent));
		if (respawn)
		{
			SCR_AutoSpawnLogic.Cast(respawn.GetSpawnLogic()).m_OnMetDiss.Insert(Stop);
			SCR_AutoSpawnLogic.Cast(respawn.GetSpawnLogic()).m_OnMetEna.Insert(Start);
		}
		
	}
	void Init(IEntity from, IEntity to)
	{
		IEntity owner = to;
		if (!owner)
			return;
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!m_pRplComponent || !m_pRplComponent.IsProxy())
		{
			SetEventMask(owner, EntityEvent.FRAME);
			
			ScriptedDamageManagerComponent damageManagerComponent = ScriptedDamageManagerComponent.Cast(owner.FindComponent(ScriptedDamageManagerComponent));
			if (damageManagerComponent)
				damageManagerComponent.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
		}
		
		m_pEventHandlerManager = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
		SCR_CharacterStaminaComponent Stam = SCR_CharacterStaminaComponent.Cast(owner.FindComponent(SCR_CharacterStaminaComponent));
		Stam.GetOnStaminaDrain().Insert(SetStaminDrain);
		ChimeraWorld world = GetGame().GetWorld();
		m_pWeather = world.GetTimeAndWeatherManager();
		m_pChar = ChimeraCharacter.Cast(owner);
		m_pDamage = m_pChar.GetDamageManager();
		//m_pLoadout = BaseLoadoutManagerComponent.Cast(m_pChar.FindComponent(BaseLoadoutManagerComponent));
		m_pLoadout = EquipedLoadoutStorageComponent.Cast(m_pChar.FindComponent(EquipedLoadoutStorageComponent));
		m_fHunger = Math.RandomInt(70, 100);
		m_fThirst = Math.RandomInt(70, 100);
		m_fEnergy = Math.RandomInt(70, 100);
		m_fTemperature = 36.6;
	}
	void Stop()
	{
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
	}
	void Start()
	{
		SetEventMask(GetOwner(), EntityEvent.FRAME);
	}
	void SetStaminDrain(float Drain)
	{
		p_Drain = p_Drain + -Drain;
	}
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_fTemperature = m_fHealthyTemperature;
		SetEventMask(owner, EntityEvent.INIT);
		SetEventMask(owner, EntityEvent.FRAME);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		if (!m_pRplComponent || !m_pRplComponent.IsProxy())
		{
			ScriptedDamageManagerComponent damageManagerComponent = ScriptedDamageManagerComponent.Cast(owner.FindComponent(ScriptedDamageManagerComponent));
			if (damageManagerComponent)
				damageManagerComponent.GetOnDamageStateChanged().Remove(OnDamageStateChanged);
		}
	}
	override bool RplSave(ScriptBitWriter writer)
	{
		return true;
	}	
	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		return true;
	}	
};
