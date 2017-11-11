
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

	Animation* current_animation = nullptr;
	Animation idle_anim;
	fPoint position;

	enum State
	{
		IDLE,
		ALERT,
	};

	Animation moving_anim;
	iRect collider;
	State state;
	fPoint speed_vect;
	bool gravity;



	bool Update(float dt);

	void Move(fPoint& position, fPoint& speed_vector) const;
	void Accelerate(fPoint& speed_vector, float x, float y) const;


};


#endif // !