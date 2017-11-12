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
			LARVA,
			LARVA_BLOCK,
		};

		Animation alert_anim;
		SDL_Rect enemyrect;
		Type type;
		bool gravity;

private:
	void LarvaBlockUpdate();
};

