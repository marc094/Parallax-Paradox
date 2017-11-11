#pragma once
#include "Entity.h"
class BaseEnemy :
	public Entity
{
public:
	BaseEnemy();
	~BaseEnemy();
	bool Update(float dt);

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

