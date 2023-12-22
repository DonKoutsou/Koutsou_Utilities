modded class SCR_CharacterCameraHandlerComponent
{
	[Attribute(defvalue: "true")]
	bool Allow3PV;

	override void OnCameraSwitchPressed()
	{
		bool current = IsInThirdPerson();
		if (Allow3PV)
			SetThirdPerson(!current);
	}
	
};