
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "j1Map.h"
#include "Rect.h"

#define ACCELERATION 0.5f
#define DECELERATION 0.1f

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
	LayerID current_layer;
	bool gravity;


	bool Awake(pugi::xml_node&);
	bool Update(float dt);

	void Move();
	void Accelerate(float x,float y);
};


#endif // !