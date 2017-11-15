
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "j1Map.h"
#include "Rect.h"

#define ACCELERATION 5.0f
#define DECELERATION 5.0f
#define GRAVITY 2.0f
#define JUMP_FORCE 20.0f

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

	Animation* current_animation = nullptr;
	Animation idle_anim;
	fPoint position;
	Animation moving_anim;
	iRect collider;
	State state;
	fPoint speed_vect;
	fPoint max_speed;
	LayerID current_layer;
	bool gravity, grounded = true;


	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual void Move(float delta_time);
	void Accelerate(float x,float y, float delta_time);
};


#endif // !