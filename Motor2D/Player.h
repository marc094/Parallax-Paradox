#pragma once
#include "Entity.h"
class Player :
	public Entity
{
public:
	Player();
	~Player();

	bool Awake();

	bool Start();
	// Called each loop iteration

	bool PreUpdate();

	bool Update(float dt);

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void SelectAnim(fPoint speed_vect);

	bool isJumping() const { return isjumping; }

	void setJumping(bool jump) { isjumping = jump; }

	void SwapLayer();
	
	fPoint GetPosition() const { return position; }

	uint GetCurrentLayer();

private:
	uint		jump_height;
	float		scale = 1.0f;
	bool		flipped;
	bool isjumping;

	p2List<Animation> animation_list;
	Animation jumping_anim;
	Animation falling_anim;
	Animation landing_anim;
	Animation changing_layers_anim;

	ColliderType current_layer;
};

