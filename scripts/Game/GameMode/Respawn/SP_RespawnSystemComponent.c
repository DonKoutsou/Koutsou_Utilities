//------------------------------------------------------------------------------------------------
class SP_RespawnSystemComponentClass : SCR_RespawnSystemComponentClass
{
};

[ComponentEditorProps(icon: HYBRID_COMPONENT_ICON)]
class SP_RespawnSystemComponent : SCR_RespawnSystemComponent
{
	SCR_SpawnLogic GetLogic(){return m_SpawnLogic;}
};
