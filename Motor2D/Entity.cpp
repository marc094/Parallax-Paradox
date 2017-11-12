#include "Entity.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "BaseEnemy.h"


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

	state = IDLE;

}

void Entity::Accelerate( float x, float y) {
	speed_vect.x += (x) / (1.0f / ACCELERATION);
	speed_vect.y += (y) / (1.0f / ACCELERATION);

	speed_vect.x = CLAMP(speed_vect.x, -5, 5);
	speed_vect.y = CLAMP(speed_vect.y, -10, 10);

	state = RUNNING;
}