class SCR_AIGetSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	float m_fRadius;
	
	[Attribute()]
	bool m_bAllowCloseSpawning;
	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string CLOSESPAWN_PORT = "AllowCloseSpawn";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radius;
	bool CloseSpawn;
	ref array <string> tags = new ref array <string>();
	ref array <Managed> correctsmartacts;
	
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		if (!GetVariableIn(CLOSESPAWN_PORT, CloseSpawn))
			CloseSpawn = m_bAllowCloseSpawning;
		if (tags.IsEmpty() && m_aTags)
			tags.Copy(m_aTags);
		if (!Radius)
			Radius = m_fRadius;
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
		if (!Origin || !Radius || tags.IsEmpty())
			return ENodeResult.FAIL;
		correctsmartacts = new array <Managed>();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		if (correctsmartacts.IsEmpty())
			return ENodeResult.FAIL;
		OutSmartAction = SCR_AISmartActionComponent.Cast(correctsmartacts.GetRandomElement());
		if (!CloseSpawn)
		{
			array <IEntity> entities = {};
			GetGame().GetTagManager().GetTagsInRange(entities, OutSmartAction.GetOwner().GetOrigin(), 25, ETagCategory.Perceivable);
			while (!entities.IsEmpty())
			{
				entities.Clear();
				OutSmartAction = SCR_AISmartActionComponent.Cast(correctsmartacts.GetRandomElement());
				GetGame().GetTagManager().GetTagsInRange(entities, OutSmartAction.GetOwner().GetOrigin(), 25, ETagCategory.Perceivable);
			}
		}
		correctsmartacts.Clear();
		if (OutSmartAction)
		{
			SetVariableOut(SMARTACTION_PORT, OutSmartAction);
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		
		return ENodeResult.FAIL;
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		
		e.FindComponents(SCR_AISmartActionComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		else
		{
			foreach (Managed Smart : smartacts)
			{
				array <string> demtags2 = new array <string>();
				SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				smatcomp.GetTags(demtags2);
				foreach (string tg : demtags2)
				{
					if (tags.Contains(tg))
						correctsmartacts.Insert(Smart);
				}
			}
		}
		return true;
	}

	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT, CLOSESPAWN_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SCR_AIGetClosestSmartAction : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	protected bool m_bForceNodeSuccess;
	
	[Attribute()]
	float m_fRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	protected static const string OUT_TAG_PORT = "OutTag";
	protected static const string OUT_CROUCH_BOOL = "CROUCH_BOOL";
	
	SCR_AISmartActionComponent OutSmartAction;
	vector Origin;
	float Radius;
	bool CloseSpawn;
	IEntity Owner;
	ref array <string> tags = new ref array <string>();
	ref array <Managed> correctsmartacts;
	ref array <string> outtags = new ref array <string>();
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		if (tags.IsEmpty() && m_aTags)
			tags.Copy(m_aTags);
		if (!Radius)
			Radius = m_fRadius;
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
		if (!Origin || !Radius || tags.IsEmpty())
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		Owner = owner.GetControlledEntity();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		if (correctsmartacts.IsEmpty())
		{
			if(m_bForceNodeSuccess)
				return ENodeResult.SUCCESS;
			return ENodeResult.FAIL;
		}
		float dist;
		foreach (Managed SmartA : correctsmartacts)
		{
			SCR_AISmartActionComponent Smart = SCR_AISmartActionComponent.Cast(SmartA);
			
			if (!dist)
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
			else if (dist > vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin()))
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
		}
		
		if (OutSmartAction)
		{
			SetVariableOut(OUT_CROUCH_BOOL, OutSmartAction.ShouldCrouchWhenPerforming);
			SetVariableOut(SMARTACTION_PORT, OutSmartAction);
			outtags.Clear();
			OutSmartAction.GetTags(outtags);
			SetVariableOut(OUT_TAG_PORT, outtags);
			correctsmartacts.Clear();
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		
		if(m_bForceNodeSuccess)
			return ENodeResult.SUCCESS;
		return ENodeResult.FAIL;
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		if (!correctsmartacts)
		{
			correctsmartacts = {};
		}
		e.FindComponents(SCR_AISmartActionComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		else
		{
			foreach (Managed Smart : smartacts)
			{
				array <string> demtags2 = new array <string>();
				SCR_AISmartActionComponent smatcomp = SCR_AISmartActionComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				
				smatcomp.GetTags(demtags2);
				foreach (string tg : demtags2)
				{
					if (tags.Contains(tg))
					{
						if (smatcomp.NeedsTest())
						{
							if (smatcomp.RunTest(Owner))
								correctsmartacts.Insert(Smart);
						}
						else
							correctsmartacts.Insert(Smart);
					}
						
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT, OUT_TAG_PORT ,OUT_CROUCH_BOOL};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SCR_AIGetClosestSmartActionSentinel : AITaskScripted
{
	[Attribute()]
	protected ref array <string> m_aTags;
	
	[Attribute()]
	float m_fRadius;

	protected static const string POISSITION_PORT = "Possition";
	protected static const string RADIUS_PORT = "Radius";
	protected static const string TAGS_PORT = "InTags";
	protected static const string SMARTACTION_PORT = "OutSmartAction";
	
	SCR_AISmartActionSentinelComponent OutSmartAction;
	vector Origin;
	float Radius;
	bool CloseSpawn;
	IEntity Owner;
	ref array <string> tags = new ref array <string>();
	ref array <Managed> correctsmartacts;
	override bool VisibleInPalette() { return true; }
	
	override string GetOnHoverDescription()
	{
		return "Smart Action in radius.";
	}
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		GetVariableIn(POISSITION_PORT, Origin);
		GetVariableIn(RADIUS_PORT, Radius);
		GetVariableIn(TAGS_PORT, tags);
		if (tags.IsEmpty() && m_aTags)
			tags.Copy(m_aTags);
		if (!Radius)
			Radius = m_fRadius;
		if (!Origin)
			Origin = owner.GetControlledEntity().GetOrigin();
		if (!Origin || !Radius || tags.IsEmpty())
			return ENodeResult.FAIL;
		Owner = owner.GetControlledEntity();
		GetGame().GetWorld().QueryEntitiesBySphere(Origin, Radius, QueryEntitiesForSmartAction);
		if (correctsmartacts.IsEmpty())
			return ENodeResult.FAIL;
		float dist;
		foreach (Managed SmartA : correctsmartacts)
		{
			SCR_AISmartActionSentinelComponent Smart = SCR_AISmartActionSentinelComponent.Cast(SmartA);
			
			if (!dist)
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
			else if (dist > vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin()))
			{
				dist = vector.Distance(Smart.m_Owner.GetOrigin(), owner.GetControlledEntity().GetOrigin());
				OutSmartAction = Smart;
			}
		}
		
		if (OutSmartAction)
		{
			SetVariableOut(SMARTACTION_PORT, OutSmartAction);
			correctsmartacts.Clear();
			OutSmartAction = null;
			return ENodeResult.SUCCESS;
		}
		
		return ENodeResult.FAIL;
	}
	private bool QueryEntitiesForSmartAction(IEntity e)
	{
		array<Managed> smartacts = new array<Managed>();
		if (!correctsmartacts)
		{
			correctsmartacts = {};
		}
		e.FindComponents(SCR_AISmartActionSentinelComponent, smartacts);
		if (smartacts.IsEmpty())
			return true;
		else
		{
			foreach (Managed Smart : smartacts)
			{
				array <string> demtags2 = new array <string>();
				SCR_AISmartActionSentinelComponent smatcomp = SCR_AISmartActionSentinelComponent.Cast(Smart);
				if (!smatcomp.IsActionAccessible())
					continue;
				smatcomp.GetTags(demtags2);
				foreach (string tg : demtags2)
				{
					if (tags.Contains(tg))
						correctsmartacts.Insert(Smart);
				}
			}
		}
		return true;
	}
	protected static ref TStringArray s_aVarsIn = { POISSITION_PORT,  RADIUS_PORT, TAGS_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { SMARTACTION_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SCR_AIGetSmartActionPerformTest : AITaskScripted
{
	protected static const string SMART_ACTION_PORT = "Samart Action";
	protected static const string TREE_PORT = "Tree";
	override bool VisibleInPalette() { return true; }
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		SCR_AISmartActionComponent smart;
		GetVariableIn(SMART_ACTION_PORT, smart);
		
		if (smart)
		{
			//SetVariableOut(SMART_ACTION_PORT, smart.GetCanBePerformedTree());
			smart = null;
			return ENodeResult.SUCCESS;
		}
		
		return ENodeResult.FAIL;
	}

	protected static ref TStringArray s_aVarsIn = { SMART_ACTION_PORT };
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { TREE_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SCR_AIGetSmartActionTagsToLook : AITaskScripted
{
	protected static const string TAGS_PORT = "TAGS";
	override bool VisibleInPalette() { return true; }
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		
		//if (smart)
		//{
		//	SetVariableOut(SMART_ACTION_PORT, smart.GetCanBePerformedTree());
		//	smart = null;
		//	return ENodeResult.SUCCESS;
		//}
		return ENodeResult.FAIL;
	}
	
	protected static ref TStringArray s_aVarsOut = { TAGS_PORT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
}
class SmartActionTest : ScriptAndConfig
{
	bool TestAction(IEntity Owner, IEntity User)
	{
		return true;
	}
}
class LanternSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		bool LightShouldBe;
		float currenttime = GetGame().GetTimeAndWeatherManager().GetTime().ToTimeOfTheDay();
		float Sunset, Sunrise;
		GetGame().GetTimeAndWeatherManager().GetSunsetHour(Sunset);
		GetGame().GetTimeAndWeatherManager().GetSunriseHour(Sunrise);
		if (Sunset < currenttime || Sunrise > currenttime)
			LightShouldBe = true;
		
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		
		bool light = m_LightComponent.IsOn();
		if(light != LightShouldBe)
		{
			return true;
		}
		return false;
	}
}
class FireplaceSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_BaseInteractiveLightComponent m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(Owner.FindComponent(SCR_BaseInteractiveLightComponent));
		if(!m_LightComponent.IsOn())
		{
			return true;
		}
		return false;
	}
}
class StoreSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(User);
		if (!char)
			return false;
		SP_StoreAISmartActionComponent storeaction = SP_StoreAISmartActionComponent.Cast(Owner.FindComponent(SP_StoreAISmartActionComponent));
		if (!storeaction)
			return false;
		int needamount;
		if (storeaction.TestDescriptor(char.GetNeed(needamount)))
		{
			return true;
		}
		return false;
	}
}
class RadioSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		RadioBroadcastComponent m_RadioComponent = RadioBroadcastComponent.Cast(Owner.FindComponent(RadioBroadcastComponent));
		return m_RadioComponent.GetState();
	}
}
class AliveSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		ScriptedDamageManagerComponent DMGMan = ScriptedDamageManagerComponent.Cast(Owner.FindComponent(ScriptedDamageManagerComponent));
		if (DMGMan.IsDestroyed())
		{
			return true;
		}
		return false;
	}
}
class DeadBodySmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(Owner);
		ScriptedDamageManagerComponent DMGMan = ScriptedDamageManagerComponent.Cast(char.GetDamageManager());
		if (!DMGMan.IsDestroyed())
		{
			return false;
		}
		
		/*WalletEntity wallet = char.GetWallet();
		if (wallet)
		{
			int ammount = wallet.GetCurrencyAmmount();
			if (ammount > 0)
			{
				return true;
			}
		}*/
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(Owner.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		foreach (IEntity item : items)
		{
			SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
			if (Unretr)
			{
				return true;
			}
			MagazineComponent mag = MagazineComponent.Cast(item.FindComponent(MagazineComponent));
			if (mag)
			{
				SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(Owner.FindComponent(SCR_AICombatComponent));
				BaseWeaponComponent weaponComp;
				BaseMagazineComponent magazineComp;
				int muzzleId;
				weaponComp = CombatComp.GetCurrentWeapon();
	
				if (weaponComp)
				{
					if (weaponComp.GetCurrentMagazine())
					{
						magazineComp = weaponComp.GetCurrentMagazine();
						if (magazineComp)
						{
							if (magazineComp.GetMagazineWell() == mag.GetMagazineWell())
							{
								return true;
							}
						}
					}
				}
			}
			//clothes
			BaseLoadoutClothComponent cloth = BaseLoadoutClothComponent.Cast(item.FindComponent(BaseLoadoutClothComponent));
			if (cloth)
			{
				//if i miss this part of clothing it means i have something to loot from here
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(Owner.FindComponent(EquipedLoadoutStorageComponent)); 
				LoadoutAreaType type = cloth.GetAreaType();
				if (!type || type.Type() == LoadoutWatchArea || type.Type() == LoadoutBinocularsArea)
				{
					continue;
				}
				if (!loadoutStorage.GetClothFromArea(type.Type()))
				{
					return true;
				}
				//look for armored helmets
				if (cloth.GetAreaType().Type() == LoadoutHeadCoverArea)
				{
					SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
					SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
					//figure out if its helmet
					EntityPrefabData ClothprefabData = item.GetPrefabData();
					ResourceName ClothprefabName = ClothprefabData.GetPrefabName();
					SCR_EntityCatalogEntry Clothentry = RequestCatalog.GetEntryWithPrefab(ClothprefabName);
					if(!Clothentry)
					{
						continue;
					}
					//see if i need helmet
					EquipedLoadoutStorageComponent myloadoutStorage = EquipedLoadoutStorageComponent.Cast(User.FindComponent(EquipedLoadoutStorageComponent)); 
					IEntity	Helmet = myloadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
					if (Helmet)
					{
						EntityPrefabData prefabData = Helmet.GetPrefabData();
						ResourceName prefabName = prefabData.GetPrefabName();
						SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
						if(!entry)
						{
							return true;
						}
					}
				}
			}
		}
		SCR_CharacterInventoryStorageComponent MyloadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(User.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_CharacterInventoryStorageComponent loadoutStorage = SCR_CharacterInventoryStorageComponent.Cast(Owner.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_InventoryStorageManagerComponent Myinv = SCR_InventoryStorageManagerComponent.Cast(User.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(Owner.FindComponent(SCR_InventoryStorageManagerComponent));
		if (loadoutStorage && MyloadoutStorage)
		{
			IEntity MyHelmet = MyloadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
			bool needhelm;
			if (MyHelmet)
			{
				EntityPrefabData prefabData = MyHelmet.GetPrefabData();
				ResourceName prefabName = prefabData.GetPrefabName();
				SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
				SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
				SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
				if(entry)
				{
					needhelm = false;
				}
				needhelm = true;
			}
			if (needhelm)
			{
				IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
				if (Helmet)
				{
					return true;
				}
			}
			IEntity MyBackpack = MyloadoutStorage.GetClothFromArea(LoadoutBackpackArea);
			if (!MyBackpack)
			{
				IEntity Backpack = loadoutStorage.GetClothFromArea(LoadoutBackpackArea);
				if (Backpack)
				{
					return true;
				}
			}
			IEntity MyArmVest = MyloadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
			if (!MyArmVest)
			{
				IEntity ArmVest = loadoutStorage.GetClothFromArea(LoadoutArmoredVestSlotArea);
				if (ArmVest)
				{
					return true;
				}
			}
			IEntity MyVest = MyloadoutStorage.GetClothFromArea(LoadoutVestArea);
			if (!MyVest)
			{
				IEntity Vest = loadoutStorage.GetClothFromArea(LoadoutVestArea);
				if (Vest)
				{
					return true;
				}
			}
			IEntity MyJacket = MyloadoutStorage.GetClothFromArea(LoadoutJacketArea);
			if (!MyJacket)
			{
				IEntity Jacket = loadoutStorage.GetClothFromArea(LoadoutJacketArea);
				if (Jacket)
				{
					return true;
				}
			}
			IEntity MyPants = MyloadoutStorage.GetClothFromArea(LoadoutPantsArea);
			if (!MyPants)
			{
				IEntity Pants = loadoutStorage.GetClothFromArea(LoadoutPantsArea);
				if (Pants)
				{
					return true;
				}
			}
			IEntity MyBoots = MyloadoutStorage.GetClothFromArea(LoadoutBootsArea);
			if (!MyBoots)
			{
				IEntity Boots = loadoutStorage.GetClothFromArea(LoadoutBootsArea);
				if (Boots)
				{
					return true;
				}
			}
		}
		
		return false;
	}
}
class AmmoRefilSmartActionTest : SmartActionTest
{
	override bool TestAction(IEntity Owner, IEntity User)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(Owner);
		if (char)
		{
			ScriptedDamageManagerComponent dmg = ScriptedDamageManagerComponent.Cast(char.GetDamageManager());
			if (!dmg.IsDestroyed())
			{
				SCR_AICombatComponent CombatComp = SCR_AICombatComponent.Cast(Owner.FindComponent(SCR_AICombatComponent));
				BaseWeaponComponent weaponComp;
				BaseMagazineComponent magazineComp;
				int muzzleId;
				weaponComp = CombatComp.GetCurrentWeapon();
	
				if (weaponComp)
				{
					if (weaponComp.GetCurrentMagazine())
					{
						magazineComp = weaponComp.GetCurrentMagazine();
					}
				}
				muzzleId = SCR_WeaponLib.GetNextMuzzleID(weaponComp);
				if (CombatComp.EvaluateLowAmmo(weaponComp, muzzleId))
				{
					return false;
				}
			}
		}
		SCR_InventoryStorageManagerComponent InvMan = SCR_InventoryStorageManagerComponent.Cast(Owner.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!InvMan)
			return false;
		
		SCR_AICombatComponent m_CombatComp = SCR_AICombatComponent.Cast(User.FindComponent(SCR_AICombatComponent));
		if (!m_CombatComp)
			return false;
		BaseWeaponComponent weaponComp;
		BaseMagazineComponent magazineComp;
		int muzzleId;
		weaponComp = m_CombatComp.GetCurrentWeapon();
		if (weaponComp)
		{
			if (weaponComp.GetCurrentMagazine())
			{
				magazineComp = weaponComp.GetCurrentMagazine();
			}
			muzzleId = SCR_WeaponLib.GetNextMuzzleID(weaponComp);
		}
		
		BaseMagazineWell iMagWell = magazineComp.GetMagazineWell();
		SCR_MagazinePredicate pred = new SCR_MagazinePredicate();
		pred.magWellType = iMagWell.Type();
		array <IEntity> OutItems = {};
		InvMan.FindItems(OutItems, pred);
		if (OutItems.IsEmpty())
			return false;
		return true;
		
	}
}
