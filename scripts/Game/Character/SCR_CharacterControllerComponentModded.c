modded class SCR_CharacterControllerComponent
{


	// Item event invokers
	ref ScriptInvoker m_OnDialogueBeganInvoker = new ref ScriptInvoker();

	//! Will be called when item use action is started
	void OnDialogueBegan(IEntity Player, IEntity Talker) { m_OnDialogueBeganInvoker.Invoke(Player, Talker); };
	
};
