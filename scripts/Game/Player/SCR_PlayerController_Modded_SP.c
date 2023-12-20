modded class SCR_PlayerController
{
	override void SetPossessedEntity(IEntity entity)
	{
		if (!m_bIsPossessing)
		{
			if (entity)
			{
				m_OnBeforePossess.Invoke(entity);
				//--- Start posessing
				m_bIsPossessing = true;

				//--- Remember previously controlled entity
				IEntity controlledEntity = GetControlledEntity();
				m_MainEntityID = RplId.Invalid();
				if (controlledEntity)
				{
					RplComponent rpl = RplComponent.Cast(controlledEntity.FindComponent(RplComponent));
					if (rpl)
					{
						rpl.GiveExt(RplIdentity.Local(), false);
						m_MainEntityID = rpl.Id();
					}
				}

				OnRplMainEntityFromID(); //--- ToDo: Remove? BumpMe should call it automatically.
				Replication.BumpMe();

				//-- Tell manager we're possessing an entity
				SCR_PossessingManagerComponent possessingManager = SCR_PossessingManagerComponent.GetInstance();
				if (possessingManager)
					possessingManager.SetMainEntity(GetPlayerId(), entity, controlledEntity, m_bIsPossessing);

				//--- Switch control
				RplComponent rpl = RplComponent.Cast(entity.FindComponent(RplComponent));
				if (rpl)
					rpl.GiveExt(GetRplIdentity(), false);
				SetAIActivation(entity, false);
				SetControlledEntity(entity);
				m_OnPossessed.Invoke(entity);
				//SetDogTagName(entity);
			}
		}
		/*else
		{
			if (!entity)
			{
				//--- Stop possessing
				m_bIsPossessing = false;

				//--- Forget main entity
				m_MainEntityID = RplId.Invalid();
				OnRplMainEntityFromID(); //--- ToDo: Remove?
				Replication.BumpMe();

				SCR_PossessingManagerComponent possessingManager = SCR_PossessingManagerComponent.GetInstance();
				if (possessingManager)
					possessingManager.SetMainEntity(GetPlayerId(), GetControlledEntity(), m_MainEntity, m_bIsPossessing);

				//--- Switch control
				IEntity controlledEntity = GetControlledEntity();
				if (controlledEntity)
				{
					RplComponent rpl = RplComponent.Cast(controlledEntity.FindComponent(RplComponent));
					if (rpl)
						rpl.GiveExt(RplIdentity.Local(), false);

					SetAIActivation(controlledEntity, true);
				}
				
				//--- Switch control
				if (m_MainEntity)
				{
					RplComponent rpl = RplComponent.Cast(m_MainEntity.FindComponent(RplComponent));
					if (rpl)
						rpl.GiveExt(GetRplIdentity(), false);
				}
				SetControlledEntity(m_MainEntity);
				m_OnPossessed.Invoke(m_MainEntity);
				SetDogTagName(entity);

				//--- SetControlledEntity(null) doesn't work yet. ToDo: Remove this check once it's implemented
				if (GetControlledEntity() != m_MainEntity)
					Print(string.Format("Error when switching control back to m_MainEntity = %1!", m_MainEntity), LogLevel.WARNING);
			}
			else
			{
				//--- Switch possessing
				SetPossessedEntity(null);
				SetPossessedEntity(entity);
				m_OnPossessed.Invoke(entity);
				SetDogTagName(entity);
			}
		}*/
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Set intial main entity of a player, for a case where an existing entity should be assigned instead of spawning a new one
	\param entity is the subject entity
	*/
	override void SetInitialMainEntity(notnull IEntity entity)
	{
		RplComponent rpl = RplComponent.Cast(entity.FindComponent(RplComponent));
		if (!rpl)
			return;

		m_MainEntityID = rpl.Id();
		OnRplMainEntityFromID();
		Replication.BumpMe();

		SCR_PossessingManagerComponent possessingManager = SCR_PossessingManagerComponent.GetInstance();
		if (possessingManager)
			possessingManager.SetMainEntity(GetPlayerId(), GetControlledEntity(), entity, m_bIsPossessing);

		rpl.GiveExt(GetRplIdentity(), false); // transfer ownership
		SetAIActivation(entity, false);
		SetControlledEntity(entity);

		m_OnPossessed.Invoke(entity);
		//SetDogTagName(entity);
	}

};
