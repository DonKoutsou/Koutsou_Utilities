enum ECharacterHitZoneGroup : EHitZoneGroup
{
	HEAD = 1,
	UPPERTORSO = 2,
	LOWERTORSO = 3,
	LEFTARM = 4,
	RIGHTARM = 5,
	LEFTLEG = 6,
	RIGHTLEG = 7
}

//------------------------------------------------------------------------------------------------
class SCR_CharacterDamageManagerComponentClass: ScriptedDamageManagerComponentClass
{
};

//------------------------------------------------------------------------------------------------
class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{
	// 1000 ms timer for bloody clothes update
	static const int BLOOD_CLOTHES_UPDATE_PERIOD = 1000;
	
	// a waterlevel of deeper than faceUnderwaterDepth meters will kill the character upon unconsciousness
	const float FACE_UNDERWATER_DEPTH = 0.3;
	
	// bleeding rate multiplier after death - used to stop particles sooner
	const float DEATH_BLEEDOUT_SCALE = 4;

	// Static array for all limbs
	static const ref array<ECharacterHitZoneGroup> LIMB_GROUPS = {ECharacterHitZoneGroup.LEFTARM, ECharacterHitZoneGroup.RIGHTARM, ECharacterHitZoneGroup.LEFTLEG, ECharacterHitZoneGroup.RIGHTLEG, ECharacterHitZoneGroup.HEAD, ECharacterHitZoneGroup.UPPERTORSO, ECharacterHitZoneGroup.LOWERTORSO};
	static const ref array<ECharacterHitZoneGroup> EXTREMITY_LIMB_GROUPS = {ECharacterHitZoneGroup.LEFTARM, ECharacterHitZoneGroup.RIGHTARM, ECharacterHitZoneGroup.LEFTLEG, ECharacterHitZoneGroup.RIGHTLEG};
	
	//replicated arrays for clients
	protected ref array<ECharacterHitZoneGroup> m_aTourniquettedGroups;
	protected ref array<ECharacterHitZoneGroup> m_aSalineBaggedGroups;
	protected ref array<float> m_aGroupBleedingRates;
	
	protected ref map<SCR_CharacterHitZone, ref SCR_ArmoredClothItemData> m_mClothItemDataMap;
	protected SCR_CharacterBloodHitZone m_pBloodHitZone;
	protected ref array<HitZone> m_aBleedingHitZones;
	protected ref map<HitZone, SCR_ParticleEmitter> m_mBleedingParticles;
	protected SCR_CharacterResilienceHitZone m_pResilienceHitZone;
	protected SCR_CharacterHeadHitZone m_pHeadHitZone;
	static protected SCR_GameModeHealthSettings s_HealthSettings;
 	
	protected bool m_bDOTScaleChangedByGM;
	protected bool m_bRegenScaleChangedByGM;
	protected bool m_bUnconsciousnessSettingsChangedByGM;
	protected bool m_bOverrideCharacterMedical;
	
	// TODO: Move these attributes to prefab data to save some memory
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Bleeding particle effect", params: "ptc", precision: 3, category: "Bleeding")]
	protected ResourceName m_sBleedingParticle;

	[Attribute(defvalue: "0.5", uiwidget: UIWidgets.Slider, desc: "Bleeding particle effect rate scale", params: "0 5 0.001", precision: 3, category: "Bleeding Mode")]
	protected float m_fBleedingParticleRateScale;

	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, desc: "Character bleeding rate multiplier", params: "0 5 0.001", precision: 3, category: "Bleeding")]
	protected float m_fDOTScale;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, desc: "Character regeneration rate multiplier", params: "0 5 0.001", precision: 3, category: "Regeneration")]
	protected float m_fRegenScale;
	
	[Attribute("0.3", UIWidgets.Auto, "Resilience regeneration scale while unconscious\n[x * 100%]")]
	protected float m_fUnconsciousRegenerationScale;
	
	[Attribute(defvalue: "true", uiwidget: UIWidgets.CheckBox, desc: "Whether unconsciousness is allowed", category: "Unconsciousness")]
	protected bool m_bPermitUnconsciousness;
	
	[Attribute(defvalue: "0.05", uiwidget: UIWidgets.Slider, desc: "Affects how much the bleeding is reduced", params: "0 1 0.001", precision: 3, category: "Tourniquets")]
	protected float m_fTourniquetStrengthMultiplier;
	
	//-----------------------------------------------------------------------------------------------------------
	event override void OnInit(IEntity owner)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode)
			return;

		s_HealthSettings = baseGameMode.GetGameModeHealthSettings();
		
#ifdef ENABLE_DIAG
		DiagInit(owner);
#endif
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Check whether character health state meets requirements for consciousness
	protected bool ShouldBeUnconscious()
	{
		HitZone bloodHZ = GetBloodHitZone();
		if (!bloodHZ)
			return false;
		
		ECharacterBloodState bloodState = bloodHZ.GetDamageState();
		if (bloodHZ.GetDamageStateThreshold(bloodState) <= bloodHZ.GetDamageStateThreshold(ECharacterBloodState.UNCONSCIOUS))	
			return true;
				
		HitZone resilienceHZ = GetResilienceHitZone();
		if (!resilienceHZ)
			return false;
		
		
		ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
		if (!character)
			return false;
		
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller)
			return false;

		ECharacterResilienceState resilienceState = resilienceHZ.GetDamageState();

		if (controller.IsUnconscious())
		{		
			if (resilienceHZ.GetDamageStateThreshold(resilienceState) <= resilienceHZ.GetDamageStateThreshold(ECharacterResilienceState.WEAKENED))
				return true;
		}
		else
		{
			if (resilienceHZ.GetDamageStateThreshold(resilienceState) <= resilienceHZ.GetDamageStateThreshold(ECharacterResilienceState.UNCONSCIOUS))
				return true;
		}
			
		return false;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void UpdateConsciousness()
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
		if (!character)
			return;
		
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller)
			return;
		
		bool unconscious = ShouldBeUnconscious();
		controller.SetUnconscious(unconscious);
		if (!unconscious)
			return;

		// If unconsciousness is not allowed, kill character
		if (!GetPermitUnconsciousness())
		{
			Kill(GetInstigatorEntity());
			return;
		}

		if (m_pBloodHitZone && m_pBloodHitZone.GetDamageState() == ECharacterBloodState.DESTROYED)
		{
			Kill(GetInstigatorEntity());
			return;
		}

		CharacterAnimationComponent animComponent = controller.GetAnimationComponent();
		if (!animComponent)
			return;

		CharacterCommandHandlerComponent commandHandlerComp = animComponent.GetCommandHandler();
		if (!commandHandlerComp)
			return;

		// To do: Create drowning feature
		vector waterLevel = commandHandlerComp.GetRelativeWaterLevel();
		if (waterLevel[0] > FACE_UNDERWATER_DEPTH)
			Kill(GetInstigatorEntity());
	}
	
	//------------------------------------------------------------------------------------------------
	void ForceUnconsciousness()
	{
		HitZone resilienceHZ = GetResilienceHitZone();
		if (!resilienceHZ)
			return;
		
		resilienceHZ.SetHealth(0);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Insert armor data from clothing item prefab into map, create map if null
	void InsertArmorData(notnull SCR_CharacterHitZone charHitZone, SCR_ArmoredClothItemData attributes)
	{
		if (!m_mClothItemDataMap)
			m_mClothItemDataMap = new map<SCR_CharacterHitZone, ref SCR_ArmoredClothItemData>();
		
		m_mClothItemDataMap.Insert(charHitZone, attributes);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Remove armor data from clothing item prefab from map, delete map if empty
	void RemoveArmorData(notnull SCR_CharacterHitZone charHitZone)
	{
		if (!m_mClothItemDataMap)
			return;
		
		if (m_mClothItemDataMap.Contains(charHitZone))
			m_mClothItemDataMap.Remove(charHitZone);
		
		if (m_mClothItemDataMap.IsEmpty())
			m_mClothItemDataMap = null;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! If !remove, take data from prefab and insert to map as class. If remove, remove this hitZone's stored data from map
	void UpdateArmorDataMap(notnull SCR_ArmoredClothItemData armorAttr, bool remove)
	{
		foreach (string hitzoneName : armorAttr.m_aProtectedHitZones)
		{
			SCR_CharacterHitZone hitZone = SCR_CharacterHitZone.Cast(GetHitZoneByName(hitzoneName));
			if (hitZone)
			{
				if (remove)
				{
					RemoveArmorData(hitZone);
					continue;
				}
				
				SCR_ArmoredClothItemData armoredClothItemData;
				armoredClothItemData = new SCR_ArmoredClothItemData(
					armorAttr.m_eProtection,
					armorAttr.m_aProtectedHitZones,
					armorAttr.m_aProtectedDamageTypes,
				);

				InsertArmorData(hitZone, armoredClothItemData);
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Get data from m_mClothItemDataMap
	SCR_ArmoredClothItemData GetArmorData(notnull SCR_CharacterHitZone charHitZone)
	{
		if (!m_mClothItemDataMap)
			return null;
		
		if (m_mClothItemDataMap.Contains(charHitZone))
			return m_mClothItemDataMap.Get(charHitZone);
		
		return null;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Get protection value of particular hitzone stored on m_mClothItemDataMap
	float GetArmorProtection(notnull SCR_CharacterHitZone charHitZone, EDamageType damageType)
	{
		SCR_ArmoredClothItemData localArmorData = GetArmorData(charHitZone);
		if (!localArmorData)
			return 0;

		foreach(EDamageType protectedType : localArmorData.m_aProtectedDamageTypes)
		{
			if (damageType == protectedType)
				return localArmorData.m_eProtection;
		}
		
		return 0;
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Invoked every time the DoT is added to certain hitZone.
	override void OnDamageOverTimeAdded(EDamageType dType, float dps, HitZone hz)
	{
		super.OnDamageOverTimeAdded(dType,  dps,  hz);

		if (hz.IsProxy())
			return;

		// Cancel regeneration when bleeding starts
		if (dType == EDamageType.REGENERATION || dType == EDamageType.HEALING)
			return;

		SCR_RegeneratingHitZone regenHZ = SCR_RegeneratingHitZone.Cast(hz);
		if (regenHZ)
			regenHZ.RemovePassiveRegeneration();
	}

	//-----------------------------------------------------------------------------------------------------------
	//!	Invoked when provided damage type is removed from certain hitZone.
	override void OnDamageOverTimeRemoved(EDamageType dType, HitZone hz)
	{
		super.OnDamageOverTimeRemoved(dType, hz);

		if (hz.IsProxy())
			return;

		// Schedule regeneration when bleeding stops
		if (dType == EDamageType.REGENERATION)
			return;

		SCR_RegeneratingHitZone regenHZ = SCR_RegeneratingHitZone.Cast(hz);
		if (regenHZ)
			regenHZ.ScheduleRegeneration();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//!	Invoked when the default hitZone changes damagestate
	override void OnDamageStateChanged(EDamageState state)
	{
		super.OnDamageStateChanged(state);
		
		if (state == EDamageState.DESTROYED)
			RemoveAllBleedingParticlesAfterDeath();
	}

	//-----------------------------------------------------------------------------------------------------------
	//!	Clean up blood particle sources after death
	void RemoveAllBleedingParticlesAfterDeath()
	{
		float bleedingRate;
		if (m_pBloodHitZone)
			bleedingRate = DEATH_BLEEDOUT_SCALE * m_pBloodHitZone.GetDamageOverTime(EDamageType.BLEEDING);

		float delay;
		if (bleedingRate > 0)
			delay = m_pBloodHitZone.GetHealth() / bleedingRate;

		// Damage over time is not simulated after character death
		GetGame().GetCallqueue().Remove(RemoveAllBleedingParticles);
		GetGame().GetCallqueue().CallLater(RemoveAllBleedingParticles, delay * 1000);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		RemoveAllBleedings();
		super.FullHeal(ignoreHealingDOT);
	}

	//-----------------------------------------------------------------------------------------------------------
	void UpdateBloodClothes()
	{
		if (GetState() == EDamageState.DESTROYED)
		{
			GetGame().GetCallqueue().Remove(UpdateBloodClothes);
			return;
		}

		int bleedingHitZonesCount;
		if (m_aBleedingHitZones)
			bleedingHitZonesCount = m_aBleedingHitZones.Count();

		if (bleedingHitZonesCount == 0)
		{
			GetGame().GetCallqueue().Remove(UpdateBloodClothes);
			return;
		}

		SCR_CharacterHitZone characterHitZone;
		for (int i; i < bleedingHitZonesCount; i++)
		{
			characterHitZone = SCR_CharacterHitZone.Cast(m_aBleedingHitZones[i]);
			if (characterHitZone)
				characterHitZone.AddBloodToClothes();
		}
	}

	//-----------------------------------------------------------------------------------------------------------
	void SetBloodHitZone(HitZone hitzone)
	{
		m_pBloodHitZone = SCR_CharacterBloodHitZone.Cast(hitzone);
	}

	//-----------------------------------------------------------------------------------------------------------
	HitZone GetBloodHitZone()
	{
		return m_pBloodHitZone;
	}
	
	//------------------------------------------------------------------------------------------------
	// Scale for effects that become worsened with blood level  
	float GetResilienceRegenScale()
	{
		if (!m_pBloodHitZone)
			return 1;

		float criticalBloodLevel = m_pBloodHitZone.GetDamageStateThreshold(ECharacterBloodState.UNCONSCIOUS);
		if (criticalBloodLevel >= 1)
			return 0;

		float currentBloodLevel = m_pBloodHitZone.GetDamageStateThreshold(m_pBloodHitZone.GetDamageState());
		if (currentBloodLevel < criticalBloodLevel)
			return 0;

		float resilienceRegenScale = (currentBloodLevel - criticalBloodLevel) / (1 - criticalBloodLevel);
		
		if (IsDamagedOverTime(EDamageType.BLEEDING) && GetIsUnconscious())
			resilienceRegenScale *= m_fUnconsciousRegenerationScale;

		return resilienceRegenScale;
	}
	void SetResilienceRegenScale(float value)
	{
		m_fUnconsciousRegenerationScale = value;
	}
	//-----------------------------------------------------------------------------------------------------------
	void GetBleedingHitZones(out notnull array<HitZone> hitZones)
	{
		if (m_aBleedingHitZones)
			hitZones.Copy(m_aBleedingHitZones);
	}

	//-----------------------------------------------------------------------------------------------------------
	void SetDOTScale(float rate, bool changed)
	{
		m_fDOTScale = rate;
		m_bDOTScaleChangedByGM = changed;
	}
	
	//------------------------------------------------------------------------------------------------
 	float GetDOTScale()
 	{
		if (!s_HealthSettings || m_bDOTScaleChangedByGM)
			return m_fDOTScale;
		
		return s_HealthSettings.GetBleedingScale();
 	}		
 	
 	//-----------------------------------------------------------------------------------------------------------
	void SetRegenScale(float rate, bool changed)
	{
		m_fRegenScale = rate;
		m_bRegenScaleChangedByGM = changed;
	}
	
	//------------------------------------------------------------------------------------------------
	// Return base regen scale
 	float GetRegenScale()
 	{
		if (!s_HealthSettings || m_bRegenScaleChangedByGM)
			return m_fRegenScale;
		
		return s_HealthSettings.GetRegenScale();
	}

	//-----------------------------------------------------------------------------------------------------------
 	bool GetPermitUnconsciousness()
 	{
		if (!s_HealthSettings || m_bUnconsciousnessSettingsChangedByGM)
			return m_bPermitUnconsciousness;
		
		return s_HealthSettings.IsUnconsciousnessPermitted();
 	}	
 	
 	//-----------------------------------------------------------------------------------------------------------
	void SetPermitUnconsciousness(bool permit, bool changed)
 	{
 		m_bPermitUnconsciousness = permit;
		m_bUnconsciousnessSettingsChangedByGM = changed;
 	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool GetIsUnconscious()
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
		if (!character)
			return false;
		
		CharacterControllerComponent controller = character.GetCharacterController();
	 	return controller && controller.IsUnconscious();
	}	
	
	//-----------------------------------------------------------------------------------------------------------
	void SetOverrideCharacterMedical(bool permit)
	{
		m_bOverrideCharacterMedical = permit;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool GetOverrideCharacterMedical()
	{
			return m_bOverrideCharacterMedical;
	}
 
 	//-----------------------------------------------------------------------------------------------------------
 	void SetResilienceHitZone(HitZone hitzone)
	{
		m_pResilienceHitZone = SCR_CharacterResilienceHitZone.Cast(hitzone);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	HitZone GetResilienceHitZone()
	{
		return m_pResilienceHitZone;
	}

	//-----------------------------------------------------------------------------------------------------------
	void SetHeadHitZone(HitZone hitzone)
	{
		m_pHeadHitZone = SCR_CharacterHeadHitZone.Cast(hitzone);
	}

	//-----------------------------------------------------------------------------------------------------------
	HitZone GetHeadHitZone()
	{
		return m_pHeadHitZone;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add immediate blood effect to clothes covering a hitzone
	\param hitZoneIndex Index of hitzone
	\param immediateBloodEffect Amount of blood effect strength increase
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_AddBloodToClothes(int hitZoneIndex, float immediateBloodEffect)
	{
		array<HitZone> hitZones = {};
		GetAllHitZones(hitZones);
		SCR_CharacterHitZone hitZone = SCR_CharacterHitZone.Cast(hitZones.Get(hitZoneIndex));
		if (hitZone)
			hitZone.AddBloodToClothes(immediateBloodEffect);
	}

	//-----------------------------------------------------------------------------------------------------------
	/*! Add bleeding from one hitzone
	\param hitZone Hitzone to get bleeding rate from and add effect to
	\param bleedingRate Rate of bleeding damage per second
	\param colliderDescriptorIndex Collider descriptor index
	*/
   	void AddBleedingHitZone(notnull SCR_CharacterHitZone hitZone, int colliderDescriptorIndex = -1)
	{
		if (!m_aBleedingHitZones)
			m_aBleedingHitZones = {};

		if (!m_aBleedingHitZones.Contains(hitZone))
			m_aBleedingHitZones.Insert(hitZone);
		
		// In case bleeding is started outside of normal context, full health will prevent DOT. This block will circumvent this issue
		float hitZoneDamageMultiplier = hitZone.GetHealthScaled();
		float bleedingRate = hitZone.GetMaxBleedingRate() - hitZone.GetMaxBleedingRate() * hitZoneDamageMultiplier;

		GetGame().GetCallqueue().CallLater(UpdateBloodClothes, BLOOD_CLOTHES_UPDATE_PERIOD, true);
		
		if (colliderDescriptorIndex == -1)
			colliderDescriptorIndex = Math.RandomInt(0, hitZone.GetNumColliderDescriptors() - 1);
		
		CreateBleedingParticleEffect(hitZone, bleedingRate, colliderDescriptorIndex);

		if (m_pBloodHitZone)
		{
			SCR_BleedingHitZoneParameters localBleedingHZParams = new SCR_BleedingHitZoneParameters(hitZone, bleedingRate);
			m_pBloodHitZone.AddBleedingHZToMap(hitZone, localBleedingHZParams);
			UpdateBleedingHitZones();
		}

		// The rest of the code is handled on authority only
		if (hitZone.IsProxy())
			return;

		array<HitZone> hitZones = {};
		GetAllHitZones(hitZones);

		int hitZoneIndex = hitZones.Find(hitZone);
		if (hitZoneIndex >= 0)
			Rpc(RpcDo_AddBleedingHitZone, hitZoneIndex, colliderDescriptorIndex, GetGroupBleedingRate(hitZone.GetHitZoneGroup()));
	}

	//-----------------------------------------------------------------------------------------------------------
	/*! Remove bleeding from one hitzone
	\param hitZone HitZone to remove bleeding from
	*/
	void RemoveBleedingHitZone(notnull HitZone hitZone)
	{
		RemoveBleedingParticleEffect(hitZone);
		if (m_aBleedingHitZones)
		{
			m_aBleedingHitZones.RemoveItem(hitZone);
			if (m_aBleedingHitZones.IsEmpty())
				m_aBleedingHitZones = null;
		}

		SCR_CharacterHitZone characterHitZone = SCR_CharacterHitZone.Cast(hitZone);
		if (characterHitZone && m_pBloodHitZone)
			m_pBloodHitZone.RemoveBleedingHZFromMap(characterHitZone);

		if (hitZone.IsProxy())
			return;
		
		hitZone.SetDamageOverTime(EDamageType.BLEEDING, 0);
		UpdateBleedingHitZones();
		
		array<HitZone> hitZones = {};
		GetAllHitZones(hitZones);
		int hitZoneIndex = hitZones.Find(hitZone);
		if (hitZoneIndex >= 0 && !GetOwner().IsDeleted())
			Rpc(RpcDo_RemoveBleedingHitZone, hitZoneIndex, GetGroupBleedingRate(characterHitZone.GetHitZoneGroup()));
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateBleedingHitZones()
	{
		SCR_CharacterHitZone hitZone;
		float bleedingDamage;
		float totalBleedingDamage;
		
		if (!m_pBloodHitZone.GetHitZoneDOTMap())
		{
			m_pBloodHitZone.SetDamageOverTime(EDamageType.BLEEDING, 0);
			m_aGroupBleedingRates = null;
			return;
		}
		
		m_aGroupBleedingRates = {0, 0, 0, 0, 0, 0, 0, 0, 0};

		foreach (SCR_BleedingHitZoneParameters localBleedingHZParams : m_pBloodHitZone.GetHitZoneDOTMap())
		{
			if (!localBleedingHZParams)
				continue;
			
			hitZone = SCR_CharacterHitZone.Cast(localBleedingHZParams.m_hHitZone);
			ECharacterHitZoneGroup hitZoneGroup = hitZone.GetHitZoneGroup();
			
			if (localBleedingHZParams.m_fDamageOverTime != 0)
				hitZone.SetDamageOverTime(EDamageType.BLEEDING, 1e-5);
			
			if (!GetGroupTourniquetted(hitZoneGroup))
				bleedingDamage = localBleedingHZParams.m_fDamageOverTime;
			else
				bleedingDamage = localBleedingHZParams.m_fDamageOverTime * m_fTourniquetStrengthMultiplier;
			
			m_aGroupBleedingRates[hitZoneGroup] = m_aGroupBleedingRates[hitZoneGroup] + localBleedingHZParams.m_fDamageOverTime;
			
			totalBleedingDamage += bleedingDamage;
		}
		
		m_pBloodHitZone.SetDamageOverTime(EDamageType.BLEEDING, totalBleedingDamage);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Create bleeding particle effect for a hitzone
	\param hitZoneIndex Index of hitzone
	\param colliderDescriptorIndex Collider descriptor index
	\param bleedingRate Bleeding rate
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_AddBleedingHitZone(int hitZoneIndex, int colliderDescriptorIndex, float groupBleedingRate)
	{
		array<HitZone> hitZones = {};
		GetAllHitZones(hitZones);
		SCR_CharacterHitZone hitZone = SCR_CharacterHitZone.Cast(hitZones.Get(hitZoneIndex));
		if (!hitZone)
			return;
		
		AddBleedingHitZone(hitZone, colliderDescriptorIndex);
	
		if (groupBleedingRate >= 0)
			SetGroupBleedingRate(hitZone.GetHitZoneGroup(), groupBleedingRate);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Remove bleeding particle effect from a hitzone
	\param hitZoneIndex Index of hitzone
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_RemoveBleedingHitZone(int hitZoneIndex, float groupBleedingRate)
	{
		array<HitZone> hitZones = {};
		GetAllHitZones(hitZones);
		SCR_CharacterHitZone hitZone = SCR_CharacterHitZone.Cast(hitZones.Get(hitZoneIndex));
		if (!hitZone)
			return;

		RemoveBleedingHitZone(hitZone);
		
		if (groupBleedingRate >= 0)
			SetGroupBleedingRate(hitZone.GetHitZoneGroup(), groupBleedingRate);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Called to start bleeding effect on a specificied bone
	void CreateBleedingParticleEffect(notnull HitZone hitZone, float bleedingRate, int colliderDescriptorIndex)
	{
		// Play Bleeding particle
		if (m_sBleedingParticle.IsEmpty())
			return;

		RemoveBleedingParticleEffect(hitZone);

		if (bleedingRate == 0 || m_fBleedingParticleRateScale == 0)
			return;

		// TODO: Blood traces on ground that should be left regardless of clothing, perhaps just delayed
		SCR_CharacterHitZone characterHitZone = SCR_CharacterHitZone.Cast(hitZone);
		if (characterHitZone.IsCovered())
			return;

		// Get bone node
		vector transform[4];
		int boneIndex;
		int boneNode;
		if (!hitZone.TryGetColliderDescription(GetOwner(), colliderDescriptorIndex, transform, boneIndex, boneNode))
			return;

		SCR_ParticleEmitter particleEmitter = SCR_ParticleEmitter.CreateAsChild(m_sBleedingParticle, GetOwner(), boneID: boneNode);
		particleEmitter.GetParticles().MultParam(-1, EmitterParam.BIRTH_RATE, bleedingRate * m_fBleedingParticleRateScale);

		if (!m_mBleedingParticles)
			m_mBleedingParticles = new map<HitZone, SCR_ParticleEmitter>;

		m_mBleedingParticles.Insert(hitZone, particleEmitter);
	}

	//-----------------------------------------------------------------------------------------------------------
	/*! Get particle effect on a specified hitzone
	\param hitZone Bleeding HitZone to get particle effect for
	*/
	void RemoveBleedingParticleEffect(HitZone hitZone)
	{
		if (!m_mBleedingParticles)
			return;

		SCR_ParticleEmitter particleEmitter = m_mBleedingParticles.Get(hitZone);
		if (particleEmitter)
		{
			particleEmitter.Pause();
			m_mBleedingParticles.Remove(hitZone);
		}

		if (m_mBleedingParticles.IsEmpty())
			m_mBleedingParticles = null;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Remove all bleeding particle effects from all bleeding hitzones
	void RemoveAllBleedingParticles()
	{
		if (!m_aBleedingHitZones || m_aBleedingHitZones.IsEmpty())
			return;
		
		foreach (HitZone hitZone : m_aBleedingHitZones)
			RemoveBleedingParticleEffect(hitZone);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Stop all bleeding
	void RemoveAllBleedings()
	{
		if (m_aBleedingHitZones)
		{
			HitZone hitZone;
			for (int i = m_aBleedingHitZones.Count() - 1; i >= 0; i--)
			{
				hitZone = m_aBleedingHitZones.Get(i);
				if (hitZone)
					RemoveBleedingHitZone(hitZone);
			}
		}

		if (m_pBloodHitZone)
			m_pBloodHitZone.SetDamageOverTime(EDamageType.BLEEDING, 0);

		SetInstigatorEntity(null);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Stop bleeding from particular hitzone group
	void RemoveGroupBleeding(ECharacterHitZoneGroup charHZGroup)
	{
		if (!m_aBleedingHitZones)
			return;
		
		if (charHZGroup == ECharacterHitZoneGroup.VIRTUAL)
			return;
		
		array<HitZone> aBleedingHitZones = {};
		aBleedingHitZones.Copy(m_aBleedingHitZones);
		SCR_CharacterHitZone charHitZone;
		foreach (HitZone hitZone : aBleedingHitZones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitZone);				
			if (!charHitZone || charHitZone.GetHitZoneGroup() != charHZGroup)
				continue;

			RemoveBleedingHitZone(hitZone);
		}

		if (!m_aBleedingHitZones || m_aBleedingHitZones.IsEmpty())
			SetInstigatorEntity(null);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Add bleeding to a particular physical hitzone
	void AddParticularBleeding(string hitZoneName = "Chest")
	{
		SCR_CharacterHitZone targetHitZone = SCR_CharacterHitZone.Cast( GetHitZoneByName(hitZoneName));

		if (!targetHitZone)
			return;
		
		if (targetHitZone.IsProxy())
			return;
		
		if (targetHitZone.GetDamageState() == EDamageState.UNDAMAGED)
			targetHitZone.SetHealthScaled(targetHitZone.GetDamageStateThreshold(ECharacterDamageState.WOUNDED));

		AddBleedingHitZone(targetHitZone);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Add bleeding to a random physical hitzone
	void AddRandomBleeding()
	{
	//	TODO@FAC figure out why AI don't heal themselves after this function is called on them
		array<HitZone> hitZones = {};
		array<HitZone> validHitZones = {};
		GetPhysicalHitZones(hitZones);

		foreach (HitZone hitZone: hitZones)
		{
			// Is not already bleeding
			if (hitZone.GetDamageOverTime(EDamageType.BLEEDING) > 0)
				continue;

			//Is character hz
			SCR_CharacterHitZone characterHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (characterHitZone)
				validHitZones.Insert(characterHitZone);
		}

		if (validHitZones.IsEmpty())
			return;

		
		SCR_CharacterHitZone hitzone = SCR_CharacterHitZone.Cast(validHitZones.GetRandomElement());
		AddBleedingHitZone(hitzone);
	}

	//-----------------------------------------------------------------------------------------------------------
	void GetGroupHitZones(ECharacterHitZoneGroup hitZoneGroup, out notnull array<HitZone> GroupHitZones)
	{
		array<HitZone> allGroupedHitZones = {};
		GetAllHitZones(allGroupedHitZones);
		foreach (HitZone hitzone : allGroupedHitZones)
		{
			SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (charHitZone && charHitZone.GetHitZoneGroup() == hitZoneGroup)
				GroupHitZones.Insert(hitzone);
		}
	}	
	
	//-----------------------------------------------------------------------------------------------------------
	ECharacterHitZoneGroup FindAssociatedHitZoneGroup(EBandagingAnimationBodyParts bodyPartToBandage)
	{
		array<HitZone> hitzones = {};
		
		GetPhysicalHitZones(hitzones);
		SCR_CharacterHitZone charHitZone;
		
		foreach (HitZone hitzone : hitzones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (charHitZone && charHitZone.GetBodyPartToHeal() == bodyPartToBandage)
				return charHitZone.GetHitZoneGroup();		
		}
				
		return null;
	}	
	
	//-----------------------------------------------------------------------------------------------------------
	EBandagingAnimationBodyParts FindAssociatedBandagingBodyPart(ECharacterHitZoneGroup hitZoneGroup)
	{
		array<HitZone> hitzones = {};
		
		GetPhysicalHitZones(hitzones);
		SCR_CharacterHitZone charHitZone;
		
		foreach (HitZone hitzone : hitzones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (charHitZone && charHitZone.GetHitZoneGroup() == hitZoneGroup)
				return charHitZone.GetBodyPartToHeal();		
		}
				
		return null;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void GetBandageAnimHitzones(EBandagingAnimationBodyParts eBandagingAnimBodyParts, out notnull array<HitZone> GroupHitZones)
	{
		array<HitZone> allGroupedHitZones = {};
		GetAllHitZones(allGroupedHitZones);
		SCR_CharacterHitZone charHitZone;
		
		foreach (HitZone hitzone : allGroupedHitZones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (charHitZone && charHitZone.GetBodyPartToHeal() == eBandagingAnimBodyParts)
				GroupHitZones.Insert(hitzone);
		}
	}
		
	//-----------------------------------------------------------------------------------------------------------
	float GetGroupHealthScaled(ECharacterHitZoneGroup hitZoneGroup)
	{
		float totalGroupHealth;
		float hitZoneQuantity;
		
		array<HitZone> allGroupedHitZones = {};
		GetAllHitZones(allGroupedHitZones);
		SCR_CharacterHitZone charHitZone;
		
		foreach (HitZone hitzone : allGroupedHitZones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (!charHitZone || charHitZone.GetHitZoneGroup() != hitZoneGroup)
				continue;
			
			totalGroupHealth += charHitZone.GetHealthScaled();	
			hitZoneQuantity ++;
		}
		
		if (hitZoneQuantity <= 0)
			return 0;
		
		return totalGroupHealth / hitZoneQuantity;
	}
	
	//------------------------------------------------------------------------------------------------
	// Get group bleeding rate for specific hitzone group
	private float GetGroupBleedingRate(ECharacterHitZoneGroup group)
	{
		if (!m_aGroupBleedingRates)
			return 0;
		
		if (group < 0 && group >= m_aGroupBleedingRates.Count())
			return 0;
		
		return m_aGroupBleedingRates[group];
	}
	
	//------------------------------------------------------------------------------------------------
	// Set bleeding rate
	/*!
	Update bleeding rates for all hitzone groups
	\param hitZoneIndex Index of hitzone
	\param colliderDescriptorIndex Collider descriptor index
	\param bleedingRate Bleeding rate
	*/
	private void SetGroupBleedingRate(ECharacterHitZoneGroup group, float rate)
	{
		if (!m_aGroupBleedingRates && rate > 0)
			m_aGroupBleedingRates = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		
		if (!m_aGroupBleedingRates)
			return;
		
		if (m_aGroupBleedingRates[group] == rate)
			return;
		
		m_aGroupBleedingRates[group] = rate;
		
		foreach (float bleedingRate: m_aGroupBleedingRates)
		{
			if (bleedingRate > 0)
				return;			
		}
		
		m_aGroupBleedingRates = null;
	}
		
	//-----------------------------------------------------------------------------------------------------------
	float GetGroupDamageOverTime(ECharacterHitZoneGroup hitZoneGroup, EDamageType damageType)
	{
		if (damageType == EDamageType.BLEEDING)
			return GetGroupBleedingRate(hitZoneGroup);

		float totalGroupDamage;
		array<HitZone> GroupedHitZones = {};
		GetGroupHitZones(hitZoneGroup, GroupedHitZones);
		SCR_CharacterHitZone charHitZone;
		
		foreach (HitZone hitzone : GroupedHitZones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (charHitZone && charHitZone.GetHitZoneGroup() == hitZoneGroup)
				totalGroupDamage += charHitZone.GetDamageOverTime(damageType);	
		}
		
		return totalGroupDamage;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool GetGroupTourniquetted(ECharacterHitZoneGroup hitZoneGroup)
	{
		return m_aTourniquettedGroups && m_aTourniquettedGroups.Contains(hitZoneGroup);
	}	
	
	
	//------------------------------------------------------------------------------------------------
	void SetTourniquettedGroup(ECharacterHitZoneGroup hitZoneGroup, bool setTourniquetted)
	{
		if (setTourniquetted)
		{
			if (!m_aTourniquettedGroups)
				m_aTourniquettedGroups = {};
			else if (m_aTourniquettedGroups.Contains(hitZoneGroup))
				return;
			
			m_aTourniquettedGroups.Insert(hitZoneGroup);
		}
		else
		{
			if (!m_aTourniquettedGroups || !m_aTourniquettedGroups.Contains(hitZoneGroup))
				return;
			else
				m_aTourniquettedGroups.RemoveItem(hitZoneGroup);
	
			if (m_aTourniquettedGroups.IsEmpty())
				m_aTourniquettedGroups = null;
		}
		
		UpdateBleedingHitZones();
		UpdateGroupDamage(hitZoneGroup);
	}

	//------------------------------------------------------------------------------------------------
	bool GetGroupSalineBagged(ECharacterHitZoneGroup hitZoneGroup)
	{
		return m_aSalineBaggedGroups && m_aSalineBaggedGroups.Contains(hitZoneGroup);
	}

	//------------------------------------------------------------------------------------------------
	void SetSalineBaggedGroup(ECharacterHitZoneGroup hitZoneGroup, bool setSalineBagged)
	{
		if (setSalineBagged)
		{
			if (!m_aSalineBaggedGroups)
				m_aSalineBaggedGroups = {};
			else if (m_aSalineBaggedGroups.Contains(hitZoneGroup))
				return;

			m_aSalineBaggedGroups.Insert(hitZoneGroup);
		}
		else
		{
			if (!m_aSalineBaggedGroups || !m_aSalineBaggedGroups.Contains(hitZoneGroup))
				return;
			else
				m_aSalineBaggedGroups.RemoveItem(hitZoneGroup);

			if (m_aSalineBaggedGroups.IsEmpty())
				m_aSalineBaggedGroups = null;
		}
	}

	//------------------------------------------------------------------------------------------------
	HitZone GetMostDOTHitZone(EDamageType damageType, bool includeVirtualHZs = false, array<EHitZoneGroup> allowedGroups = null)
	{
		float highestDOT;
		float localDOT;
		HitZone highestDOTHitZone;
		array<HitZone> hitzones = {};
		
		if (includeVirtualHZs)
			GetAllHitZones(hitzones);
		else
			GetPhysicalHitZones(hitzones);
		
		foreach (HitZone hitzone : hitzones)
		{
			if (allowedGroups)
			{
				SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitzone);
				if (!charHitZone)
					continue;
				
				if (!allowedGroups.Contains(charHitZone.GetHitZoneGroup()))
					continue;
			}
			
			localDOT = hitzone.GetDamageOverTime(damageType);
			if (localDOT > highestDOT)
			{
				highestDOT = localDOT;
				highestDOTHitZone = hitzone;
			}
		}
		return highestDOTHitZone;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	static void GetAllLimbs(notnull out array<ECharacterHitZoneGroup> limbs)
	{
		limbs.Copy(LIMB_GROUPS);
	}	
	
	//-----------------------------------------------------------------------------------------------------------
	static void GetAllExtremities(notnull out array<ECharacterHitZoneGroup> limbs)
	{
		limbs.Copy(EXTREMITY_LIMB_GROUPS);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void UpdateGroupDamage(ECharacterHitZoneGroup hitZoneGroup)
	{
		float AIMING_DAMAGE_MULTIPLIER = 4;
		float TOURNIQUETTED_LEG_DAMAGE = 0.7;
		float TOURNIQUETTED_ARMS_DAMAGE = 0.7;
		// Updated OnDamageStateChanged, movement damage is equal to the accumulative damage of all hitzones in the limb divided by the amount of limbs of type
		if (hitZoneGroup == ECharacterHitZoneGroup.LEFTLEG || hitZoneGroup == ECharacterHitZoneGroup.RIGHTLEG)
		{
			float leftLegDamage;
			if (GetGroupTourniquetted(ECharacterHitZoneGroup.LEFTLEG))
				leftLegDamage = TOURNIQUETTED_LEG_DAMAGE;
			else
				leftLegDamage = (1 - GetGroupHealthScaled(ECharacterHitZoneGroup.LEFTLEG));
			
			float rightLegDamage;
			if (GetGroupTourniquetted(ECharacterHitZoneGroup.RIGHTLEG))
				rightLegDamage = TOURNIQUETTED_LEG_DAMAGE;			
			else
				rightLegDamage = (1 - GetGroupHealthScaled(ECharacterHitZoneGroup.RIGHTLEG));

			float legDamage = (leftLegDamage + rightLegDamage) * 0.5;
			SetMovementDamage( legDamage );
		}

		if (hitZoneGroup == ECharacterHitZoneGroup.LEFTARM || hitZoneGroup == ECharacterHitZoneGroup.RIGHTARM)
		{
			float leftArmDamage;
			if (GetGroupTourniquetted(ECharacterHitZoneGroup.LEFTARM))
				leftArmDamage = TOURNIQUETTED_ARMS_DAMAGE;			
			else
				leftArmDamage = (1 - GetGroupHealthScaled(ECharacterHitZoneGroup.LEFTARM));

			float rightArmDamage;
			if (GetGroupTourniquetted(ECharacterHitZoneGroup.RIGHTARM))
				rightArmDamage = TOURNIQUETTED_ARMS_DAMAGE;
			else
				rightArmDamage = (1 - GetGroupHealthScaled(ECharacterHitZoneGroup.RIGHTARM));

			float armDamage = (leftArmDamage + rightArmDamage) * 0.5;
			SetAimingDamage(armDamage * AIMING_DAMAGE_MULTIPLIER);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	/*! Determine which hitzone group is taking highest DOT
	\param damageTypes Which damagetype will be checked on the hitzone for highest damage amount
	\param onlyExtremities Only compare the 4 extremities (both arms, both legs)
	\param ignoreTQdHitZones Ignore tourniquetted limbs when looking for the one most bleeding one
	*/
	ECharacterHitZoneGroup GetMostDOTHitzoneGroup(EDamageType damageType, bool onlyExtremities = false, bool ignoreTQdHitZones = false)
	{
		if (!m_aBleedingHitZones)
			return null;

		array<float> DOTValues = {};
		typename groupEnum = ECharacterHitZoneGroup;
		int groupCount = groupEnum.GetVariableCount();

		for (int i; i < groupCount; i++)
			DOTValues.Insert(0);

		float highestDOT;
		float localDOT;
		array<HitZone> hitZones = {}; 
		
		GetPhysicalHitZones(hitZones);
		SCR_CharacterHitZone charHitZone;
		ECharacterHitZoneGroup group;
		
		foreach (HitZone hitzone : hitZones)
		{
			charHitZone = SCR_CharacterHitZone.Cast(hitzone);
			if (!charHitZone)
				continue;
		
			// Virtual hitzones don't count to the accumulation of bleedings
			group = charHitZone.GetHitZoneGroup();
			if (group == EHitZoneGroup.VIRTUAL)
				continue;
			
			// If only extremities are desired for checking, continue if group is not an extremity
			if (onlyExtremities && !EXTREMITY_LIMB_GROUPS.Contains(group))
				continue;

			float DOT = m_pBloodHitZone.GetPhysicalHZBleedingDOT(charHitZone);
			
			// GetPhysicalHZBleedingDOT() is unaware of tourniquet status, so it is reduced seperately here.
			if (GetGroupTourniquetted(group))
			{
				DOT *= m_fTourniquetStrengthMultiplier;
				if (ignoreTQdHitZones)
				{
					DOTValues[group] = 0;
					continue;
				}
			}
			
			DOTValues[group] = DOTValues[group] + DOT;
		}
		
		ECharacterHitZoneGroup mostDOTHitZoneGroup;
		
		for (int i; i < groupCount; i++)
		{
			if (DOTValues[i] > highestDOT)
			{
				highestDOT = DOTValues[i];
				mostDOTHitZoneGroup = i;
			}
		}
		
		return mostDOTHitZoneGroup;
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Get bone name for given hz group
	string GetBoneName(ECharacterHitZoneGroup hzGroup)
	{
		switch (hzGroup)
		{
			case ECharacterHitZoneGroup.HEAD:
			{
				return "Head";
			} break;

			case ECharacterHitZoneGroup.UPPERTORSO:
			{
				return "Spine5";
			} break;

			case ECharacterHitZoneGroup.LOWERTORSO:
			{
				return "Spine1";
			} break;

			case ECharacterHitZoneGroup.LEFTARM:
			{
				return "LeftForeArm";
			} break;

			case ECharacterHitZoneGroup.RIGHTARM:
			{
				return "RightForeArm";
			} break;

			case ECharacterHitZoneGroup.LEFTLEG:
			{
				return "LeftKnee";
			} break;

			case ECharacterHitZoneGroup.RIGHTLEG:
			{
				return "RightKnee";
			}
		}

		return string.Empty;
	}
	
#ifdef ENABLE_DIAG		
	//------------------------------------------------------------------------------------------------ 
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_CHARACTER_LOG_PLAYER_DAMAGE,"","Log player damage","GameCode");
		
		if (System.IsCLIParam("logPlayerDamage"))
			DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_CHARACTER_LOG_PLAYER_DAMAGE, true);
	}

	//-----------------------------------------------------------------------------------------------------------
	protected override void OnDamage(
				EDamageType type,
				float damage,
				HitZone pHitZone,
				IEntity instigator,
				inout vector hitTransform[3],
				float speed,
				int colliderID,
				int nodeID)
	{
		super.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);

		if (DiagMenu.GetBool(SCR_DebugMenuID.DEBUGUI_CHARACTER_LOG_PLAYER_DAMAGE))
		{
			ScriptedHitZone scriptedHz = ScriptedHitZone.Cast(pHitZone);
			if (!scriptedHz)
				return;

			IEntity hzOwner = scriptedHz.GetOwner();
			if (!hzOwner)
				return;
			
			string instigatorName;
			int instigatorID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(instigator);
			if (instigatorID > 0)
			{
				instigatorName = GetGame().GetPlayerManager().GetPlayerName(instigatorID);
			}
			else
			{
				ResourceName prefabName;
				if (instigator)
				{
					EntityPrefabData prefabData = instigator.GetPrefabData();
					if (prefabData)
						prefabName = prefabData.GetPrefabName();
				}
				
				if (prefabName.IsEmpty())
				{
					if (instigator)
					{
						instigatorName = ((instigator.GetID()).ToString());
					}
					else
					{
					instigatorName = instigator.ToString();
					}
				}
				else
				{
					TStringArray strs = new TStringArray;
					prefabName.Split("/", strs, true);
					instigatorName = ((instigator.GetID()).ToString()) + strs[strs.Count() - 1];
				}
			}
			
			string hzOwnerName;
			int hzOwnerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(hzOwner);
			if (hzOwnerID > 0)
			{
				hzOwnerName = GetGame().GetPlayerManager().GetPlayerName(hzOwnerID);
			}
			else
			{
				EntityPrefabData prefabData = hzOwner.GetPrefabData();
				ResourceName prefabName = prefabData.GetPrefabName();
				
				if (prefabName.IsEmpty())
				{
					hzOwnerName = ((hzOwner.GetID()).ToString());
				}
				else
				{
					TStringArray strs = new TStringArray;
					prefabName.Split("/", strs, true);
					hzOwnerName = ((hzOwner.GetID()).ToString()) + strs[strs.Count() - 1];
				}
			}

			if (EntityUtils.IsPlayer(instigator) || EntityUtils.IsPlayer(hzOwner))
				PrintFormat("HIT LOG: (%1) damaged (%2) - [Damage = %3, Speed = %4]", instigatorName, hzOwnerName, damage, speed);
		}
	}
#endif		
	
	
#ifdef ENABLE_DIAG
	//-----------------------------------------------------------------------------------------------------------
	void DiagInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.DIAG);
		// Register to debug menu
		DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_CHARACTER_DAMAGE,"","Deal damage debug","Character");
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_CHARACTER_DAMAGE,0);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnDiag(IEntity owner, float timeSlice)
	{
		ProcessDebug(owner);
	}

	//------------------------------------------------------------------------------------------------
	void ProcessDebug(IEntity owner)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.DEBUGUI_CHARACTER_DAMAGE))
		{
			// Only apply debug damages to currently controlled character
			IEntity playerEntity = GetGame().GetPlayerController().GetControlledEntity();
			if (playerEntity != owner)
				return;
			
			DbgUI.Text("See: GameCode >> Hit Zones >> Player HitZones for damages");
			DbgUI.Text("Applying damage from client is NOT possible");
			DbgUI.Text("Y to reduce head health by 10");
			DbgUI.Text("U to reduce chest health by 10");
			DbgUI.Text("I to reduce right thigh health by 10");
			DbgUI.Text("O to reduce left thigh health by 10");
			DbgUI.Text("P to add bleeding to right thigh");
			DbgUI.Text("T to add bleeding to left thigh");
			DbgUI.Text("K to reset damage effect");
		
			vector hitPosDirNorm[3];
			
			if (Debug.KeyState(KeyCode.KC_Y))
			{
				Debug.ClearKey(KeyCode.KC_Y);
				HandleDamage(EDamageType.TRUE, 10, hitPosDirNorm, GetGame().GetPlayerController().GetControlledEntity(), m_pHeadHitZone, GetGame().GetPlayerController().GetControlledEntity(), null, -1, -1);
			}
			if (Debug.KeyState(KeyCode.KC_U))
			{
				Debug.ClearKey(KeyCode.KC_U);
				HandleDamage(EDamageType.TRUE, 10, hitPosDirNorm, GetGame().GetPlayerController().GetControlledEntity(), GetHitZoneByName("Chest"), GetGame().GetPlayerController().GetControlledEntity(), null, -1, -1);
			}
			if (Debug.KeyState(KeyCode.KC_I))
			{
				Debug.ClearKey(KeyCode.KC_I);
				HandleDamage(EDamageType.TRUE, 10, hitPosDirNorm, GetGame().GetPlayerController().GetControlledEntity(), GetHitZoneByName("RThigh"), GetGame().GetPlayerController().GetControlledEntity(), null, -1, -1);
			}
			if (Debug.KeyState(KeyCode.KC_O))
			{
				Debug.ClearKey(KeyCode.KC_O);
				HandleDamage(EDamageType.TRUE, 10, hitPosDirNorm, GetGame().GetPlayerController().GetControlledEntity(), GetHitZoneByName("LThigh"), GetGame().GetPlayerController().GetControlledEntity(), null, -1, -1);
			}
			if (Debug.KeyState(KeyCode.KC_P))
			{
				Debug.ClearKey(KeyCode.KC_P);
				AddParticularBleeding("RThigh");
			}
			if (Debug.KeyState(KeyCode.KC_T))
			{
				Debug.ClearKey(KeyCode.KC_T);
				AddParticularBleeding("LThigh");
			}
			if (Debug.KeyState(KeyCode.KC_K))
			{
				Debug.ClearKey(KeyCode.KC_K);
				FullHeal();
			}
		}
	}
#endif
};
