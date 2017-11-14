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

	void Init();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();

	// Called each loop iteration
	virtual bool PreUpdate();

	// Called each loop iteration
	virtual bool Update(float dt);

	// Called each loop iteration
	virtual bool PostUpdate();

	// Called before quitting
	virtual bool CleanUp();

	virtual bool Load(pugi::xml_node&);

	virtual bool Save(pugi::xml_node&) const;
	void Checkcollisions(LayerID collidertype, iRect rect_frame, fPoint position , fPoint* speed_vector) const;
	bool DoCollide(const iRect collider1, const iRect collider2) const;
	void SetSpVecToCollisions(const iRect collider, const iRect entity, fPoint &speed_vector) const;
	void BlitDebugColliders() const;

private:

	int scale;
};
#endif