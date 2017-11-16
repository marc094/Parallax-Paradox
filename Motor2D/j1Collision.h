#ifndef __j1COLLISION__
#define __j1COLLISION__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"
#include "p2Point.h"
#include "Rect.h"

enum LayerID {
	BACK_LAYER,
	FRONT_LAYER,
	DECORATION_LAYER,
	LAYER_AMOUNT
};

struct Collider
{
	iRect rect;
	LayerID collidertype;

};

class j1Collision :
	public j1Module
{
public:
	j1Collision();
	~j1Collision();
	
	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool PreUpdate() override;

	// Called each loop iteration
	bool Update(float dt) override;

	// Called each loop iteration
	bool PostUpdate() override;

	// Called before quitting
	bool CleanUp() override;

	bool Load(pugi::xml_node&) override;

	bool Save(pugi::xml_node&) const override;

	bool Checkcollisions(LayerID collidertype, iRect rect_frame, fPoint position , fPoint& speed_vector, float delta_time) const;
	bool DoCollide(const iRect collider1, const iRect collider2) const;
	void SetSpVecToCollisions(const iRect collider, const iRect entity, fPoint &speed_vector, bool& grounded, float delta_time) const;
	void BlitDebugColliders() const;

private:
	int scale = 0;
};
#endif