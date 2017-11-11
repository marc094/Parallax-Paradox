#include "Entity.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "BaseEnemy.h"
#include "j1Player.h"

Entity::Entity()
{
}


Entity::~Entity()
{
}

bool Entity::Update(float dt)
{
	
	return true;
}

void Entity::Move(fPoint& position, fPoint& speed_vector) const {


	position.x += speed_vector.x;
	position.y += speed_vector.y;


	speed_vector.x = REDUCE_TO(speed_vector.x, 0, DECELERATION * 2);
	//speed_vector.y = REDUCE_TO(speed_vector.y, 0, DECELERATION);

}

void Entity::Accelerate(fPoint& speed_vector, float x, float y) const {
	speed_vector.x += (x) / (1.0f / ACCELERATION);
	speed_vector.y += (y) / (1.0f / ACCELERATION);

	//speed_vector->x = CLAMP(speed_vector.x, -5, 5);
	//speed_vector->y = CLAMP(speed_vector.y, -10, 10);

}