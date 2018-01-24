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
		SCROLL_LEFT,
		LOADING_SCREEN
	};


	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool PreUpdate() override;

	// Called each loop iteration
	bool Update(float dt) override;

	j1Transition();
	~j1Transition();

	Transition TransitionType(Transition t = Transition::NONE);
	bool MakeTransition(Callback _cb, Transition _type = Transition::NONE, float time = 1.0f);

	enum FadeState {
		NOT_FADING = -1,
		FADING_OUT,
		FADING_IN
	} fade_state = FadeState::NOT_FADING;

private:

	void Fade_To_Black();
	void Scrolling();
	void LoadScreen();
	
	enum ScrollState {
		NOT_SCROLLING = -1,
		SNAPSHOT,
		SCROLLING
	} scroll_state = NOT_SCROLLING;

	bool			completed = true;
	float			transition_time = 0.0f, transcurred_time = 0.0f;
	int				speed = 0;
	float			fade_alpha = 0;

	SDL_Texture*	scroll_transition_texture = nullptr;
	SDL_Texture*	loading_transition_texture = nullptr;
	SDL_Rect		dst_rect;

	Transition type = NONE;

	Callback cb = nullptr;
};

#endif