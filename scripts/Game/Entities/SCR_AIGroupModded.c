modded class SCR_AIGroup
{
	override void OnLeaderChanged(AIAgent currentLeader, AIAgent prevLeader)
	{
		Event_OnLeaderChanged.Invoke(currentLeader, prevLeader);
		
		if (currentLeader && currentLeader.GetControlledEntity())
		{
			EventHandlerManagerComponent eventHandlerManagerComponent = EventHandlerManagerComponent.Cast(currentLeader.GetControlledEntity().FindComponent(EventHandlerManagerComponent));
			if (eventHandlerManagerComponent)
				eventHandlerManagerComponent.RegisterScriptHandler("OnConsciousnessChanged", this, this.OnLeaderConsciousnessChanged, true);
		}
		
		if (prevLeader && prevLeader.GetControlledEntity())
		{
			EventHandlerManagerComponent eventHandlerManagerComponent = EventHandlerManagerComponent.Cast(prevLeader.GetControlledEntity().FindComponent(EventHandlerManagerComponent));
			if (eventHandlerManagerComponent)
				eventHandlerManagerComponent.RemoveScriptHandler("OnConsciousnessChanged", this, this.OnLeaderConsciousnessChanged, true);
		}
		SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(currentLeader.GetControlledEntity().FindComponent(SCR_CharacterRankComponent));
		SCR_ECharacterRank m_CurrentRank = Rankcomp.GetCharacterRank(currentLeader.GetControlledEntity());
		switch (m_CurrentRank)
		{
		case SCR_ECharacterRank.PRIVATE:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.CORPORAL);
			}
			break;
		case SCR_ECharacterRank.CORPORAL:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.SERGEANT);
			}
			break;
		case SCR_ECharacterRank.SERGEANT:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.LIEUTENANT);
			}
			break;
		case SCR_ECharacterRank.LIEUTENANT:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.CAPTAIN);
			}
			break;
		case SCR_ECharacterRank.CAPTAIN:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.MAJOR);
			}
			break;
		case SCR_ECharacterRank.MAJOR:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.COLONEL);
			}
			break;
		case SCR_ECharacterRank.COLONEL:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.COMMANDER);
			}
			break;
		}
	}
};