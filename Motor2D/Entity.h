#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Animation.h"
#include "Rect.h"

#define ACCELERATION 7.5f * 60
#define DECELERATION 15.0f * 60
#define GRAVITY 2.2f * 1.6f
#define JUMP_FORCE 3300.f
#define MAX_SPEED_X 240.f
#define MAX_SPEED_Y 480.f
#define ENEMY_MAX_SPEED_X 1.5f * 60
#define ENEMY_MAX_SPEED_Y 3.0f * 60

class Entity 
{
public:
	Entity();
	~Entity();


	enum State
	{
		IDLE,
		JUMPING = 0x00000001,
		FALLING = 0x00000010,
		LANDING = 0x00000100,
		RUNNING = 0x00001000,
		CHANGING_LAYER = 0x00010000,
		ALERT = 0x00100000,
	};

	State state = IDLE;
	fPoint position;
	fPoint speed_vect;
	fPoint max_speed;
	float camera_speed = 1.0f;
	Animation* current_animation = nullptr;
	Animation idle_anim;
	Animation moving_anim;
	Collider collider;
	LayerID current_layer;
	bool gravity = true;
	bool grounded = true;

	bool to_delete = false;


	virtual void Move(float delta_time);
	virtual void Break(float delta_time);
	virtual void Accelerate(float x, float y, float delta_time);
};
#endif // !