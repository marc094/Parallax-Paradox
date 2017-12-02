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

void Entity::Move(float delta_time) {
	position.x += speed_vect.x * delta_time;
	position.y += speed_vect.y * delta_time;
}

void Entity::Break(float delta_time) {
	speed_vect.x = Interpolate(speed_vect.x, 0.0f, DECELERATION * delta_time);
}

void Entity::Accelerate( float x, float y, float delta_time) {
	speed_vect.x += x * ACCELERATION * delta_time;
	speed_vect.y += y * ACCELERATION * delta_time;

	speed_vect.x = CLAMP(speed_vect.x, -max_speed.x, max_speed.x);
	speed_vect.y = CLAMP(speed_vect.y, -max_speed.y, max_speed.y);
}