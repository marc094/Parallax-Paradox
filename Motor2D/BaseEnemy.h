#pragma once
#include "InteractiveEntity.h"
class BaseEnemy :
	public InteractiveEntity
{
public:
	BaseEnemy();
	~BaseEnemy();
	
		enum Type
		{
			GROUND,
			BOXER,
			AIR,
		};

		Animation alert_anim;
		fPoint position;
		ColliderType currentLayer;
		SDL_Rect enemyrect;
		Type type;
		bool gravity;
};

