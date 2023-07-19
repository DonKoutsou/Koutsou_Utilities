class FoodEntityClass: GameEntityClass
{

}
class FoodEntity: GameEntity
{
	
	float cokkingstate = 120;
	bool Cook(float amount)
	{
		if(cokkingstate <= 0)
			return true;
		cokkingstate = cokkingstate - amount;
		return false;
	}
	
}