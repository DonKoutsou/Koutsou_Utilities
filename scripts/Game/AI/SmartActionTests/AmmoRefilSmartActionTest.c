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