#pragma once
#include "Entity.h"

class BaseEnemy :
	public Entity
{
public:
	BaseEnemy();
	~BaseEnemy();
	bool Start();
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
	Type type = Type::GROUND;
	bool gravity = false;

	float				update_to_frame_ratio = 0.1f;
	unsigned int		accumulated_updates = 0;
	unsigned int		current_path_index = 0;

private:
	void LarvaBlockUpdate(float dt);
	void GetPath();
	void FollowPath(float delta_time);

	p2DynArray<iPoint> path;
};

