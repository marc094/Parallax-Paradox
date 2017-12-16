#ifndef __Module_Transition_H__
#define __Module_Transition_H__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

struct SDL_Texture;

class j1Transition :
	public j1Module
{
public:
	enum Transition {
		NONE = -1,
		FADE_TO_BLACK,
		SCROLL_RIGHT,
		SCROLL_LEFT
	};


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

	j1Transition();
	~j1Transition();

	Transition TransitionType(Transition t = NONE);

private:

	float		transition_time = 0.0f;
	uint		speed = 0;

	SDL_Texture* transition_tex = nullptr;
	SDL_Rect dst_rect;

	Transition type;
};

#endif