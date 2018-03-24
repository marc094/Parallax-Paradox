#ifndef __j1COLLISION__
#define __j1COLLISION__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"
#include "p2Point.h"
#include "Rect.h"

class Entity;

enum LayerID {
	BACK_LAYER,
	FRONT_LAYER,
	DECORATION_LAYER,
	LAYER_AMOUNT
};

struct Collider
{
	iRect rect;
	bool dynamic = false;
	bool sensor = false;
	Entity* entity = nullptr;
	LayerID layer;
};

class j1Collision :
	public j1Module
{
public:
	j1Collision();
	~j1Collision();

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool PostUpdate() override;

	iPoint Checkcollisions(Collider c1, float delta_time) const;

private:
	bool DoCollide(const iRect collider1, const iRect collider2) const;
	iPoint GetCollisionSide(const iRect entity, const iRect collider, fPoint speed_vector, float delta_time) const;
	void BlitDebugColliders() const;

private:
	float scale = 1.0f;
};
#endif