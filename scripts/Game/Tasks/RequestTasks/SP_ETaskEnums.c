//------------------------------------------------------------------------------------------------------------//
enum ETaskTimeLimmit
{
	HOURS,
	DAYS,
	WEEKS,
	YEARS,
}
//------------------------------------------------------------------------------------------------------------//
enum ETaskType
{
	none,
	KILL,
	DELIVER,
	NAVIGATE,
	RESCUE,
	BOUNTY,
	RETRIEVE
}
//------------------------------------------------------------------------------------------------------------//
enum ETaskState
{
	EMPTY,
	UNASSIGNED,
	ASSIGNED,
	COMPLETED,
	FAILED,
}