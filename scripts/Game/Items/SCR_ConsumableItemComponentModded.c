
modded class SCR_ConsumableItemComponent
{
	[Attribute("0", UIWidgets.CheckBox, "Can it be eated")]
	protected bool m_bIsEatable;
	
	[Attribute("0", UIWidgets.CheckBox, "Can it be drinked")]
	protected bool m_bIsDrinkable;
	
	[Attribute("0", UIWidgets.CheckBox, "Amount of hunger/thirst this consumable holds")]
	protected int m_fInitContainerCapacity;
	
	[Attribute("0", UIWidgets.CheckBox, "Amount of hunger/thirst this consumable will replenish on use")]
	protected float m_fUseAmount;

	protected float CurrentCapacity;
	
	float GetUseAmount()
	{
		return m_fUseAmount;
	}
	int GetInitialCapacity()
		return m_fInitContainerCapacity;
	float GetCurrentCapacity()
		return CurrentCapacity;
	bool IsEatable()
	{
		return m_bIsEatable;
	}
	
	bool IsDrinkable()
	{
		return m_bIsDrinkable;
		
	}
	
	bool HasUses()
	{
		if(CurrentCapacity > 0)
		{
			return true;
		}
		return false;
	}
	void Use(float amount)
	{
		CurrentCapacity = CurrentCapacity - amount;
	}
	void Refill()
	{
		CurrentCapacity = m_fInitContainerCapacity;
		SoundComponent sound = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		if (sound)
		{
			sound.SoundEvent("SOUND_REFIL");
		}
	}
	override event protected void EOnInit(IEntity owner)
	{
		if(!CurrentCapacity)
		{
			CurrentCapacity = m_fInitContainerCapacity;
		}
	}
};