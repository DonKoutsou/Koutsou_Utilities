class SCR_AIGetCurrentWeaponProperties : AITaskScripted
{
	protected const static string WEAPON_COMPONENT_PORT = "WeaponComponent";	
	protected const static string MAGAZINE_COMPONENT_PORT = "MagazineComponent";	
	protected const static string MUZZLE_ID_PORT = "MuzzleId";
	protected const static string CHARACTER_PORT = "Character";
	
	
	protected static ref TStringArray s_aVarsIn = {CHARACTER_PORT};
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = {WEAPON_COMPONENT_PORT, MAGAZINE_COMPONENT_PORT, MUZZLE_ID_PORT};
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		BaseWeaponComponent weaponComp;
		BaseMagazineComponent magazineComp;
		int muzzleId;
		IEntity Char;
		GetVariableIn(CHARACTER_PORT, Char);
		if (!Char)
			Char = owner.GetControlledEntity();
		
		SCR_AICombatComponent m_CombatComp = SCR_AICombatComponent.Cast(Char.FindComponent(SCR_AICombatComponent));
		if (!m_CombatComp)
			return ENodeResult.FAIL;
		weaponComp = m_CombatComp.GetCurrentWeapon();
	
		if (weaponComp)
		{
			if (weaponComp.GetCurrentMagazine())
			{
				magazineComp = weaponComp.GetCurrentMagazine();
			}
			muzzleId = SCR_WeaponLib.GetNextMuzzleID(weaponComp);
		}
		if (!weaponComp || !muzzleId || !magazineComp)
			return ENodeResult.FAIL;
		
		SetVariableOut(WEAPON_COMPONENT_PORT, weaponComp);
		SetVariableOut(MAGAZINE_COMPONENT_PORT, magazineComp);
		SetVariableOut(MUZZLE_ID_PORT, muzzleId);
		return ENodeResult.SUCCESS;
	}
	override bool VisibleInPalette() { return true; }
	
}

