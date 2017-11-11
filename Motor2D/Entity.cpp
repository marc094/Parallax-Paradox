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

bool Entity::Awake(pugi::xml_node&)
{

	return true;
}
bool Entity::Update(float dt)
{
	
	return true;
}

void Entity::Move() {

	position.x += speed_vect.x;
	position.y += speed_vect.y;


	speed_vect.x = REDUCE_TO(speed_vect.x, 0, DECELERATION * 2);
	//speed_vector.y = REDUCE_TO(speed_vector.y, 0, DECELERATION);

}

void Entity::Accelerate( float x, float y) {
	speed_vect.x += (x) / (1.0f / ACCELERATION);
	speed_vect.y += (y) / (1.0f / ACCELERATION);

	//speed_vector->x = CLAMP(speed_vector.x, -5, 5);
	//speed_vector->y = CLAMP(speed_vector.y, -10, 10);

}