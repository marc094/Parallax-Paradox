#pragma once
#include "j1Entities.h"
class InteractiveEntity :
	public j1Entities
{
public:
	InteractiveEntity();
	~InteractiveEntity();



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
	


	void Init()
	{
		active = true;
	}

};

