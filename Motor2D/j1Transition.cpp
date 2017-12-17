#include "j1Transition.h"
#include "SDL\include\SDL.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Textures.h"


j1Transition::j1Transition()
{
	name.create("transition");
}


j1Transition::~j1Transition()
{
}

j1Transition::Transition j1Transition::TransitionType(j1Transition::Transition t)
{
	if (t == NONE) return type;	
	type = t;
	return Transition::NONE;
}

bool j1Transition::MakeTransition(Callback _cb, Transition _type, float time)
{
	completed = false;
	cb = _cb;
	transition_time = time;
	transcurred_time = 0.0f;
	type = _type;
	uint w = 0, h = 0;
	App->win->GetWindowSize(w, h);
	dst_rect.x = 0;
	dst_rect.y = 0;
	switch (type)
	{
	case j1Transition::NONE:
		break;
	case j1Transition::FADE_TO_BLACK:
		fade_state = FADING_OUT;
		break;
	case j1Transition::SCROLL_RIGHT:
		speed = (int)(w / time);
	case j1Transition::SCROLL_LEFT:
		scroll_state = SNAPSHOT;
		speed = -(int)(w / time);
		break;
	case j1Transition::LOADING_SCREEN:
		break;
	default:
		break;
	}
	return false;
}

void j1Transition::Fade_To_Black()
{
	fade_alpha = MIN(1.0f, transcurred_time / transition_time);
	switch (fade_state)
	{
	case j1Transition::FADING_OUT:
		if (transcurred_time > transition_time) {
			fade_state = FADING_IN;
			transcurred_time = -0.6f;
			if (cb != nullptr) {
				cb(0);
				cb = nullptr;
			}
		}
		break;
	case j1Transition::FADING_IN:
		fade_alpha = 1.0f - fade_alpha;
		if (transcurred_time > transition_time) {
			fade_alpha = 0;
			fade_state = NOT_FADING;
			completed = true;
		}
		break;
	default:
		break;
	}
	App->render->DrawQuad(dst_rect, 0, 0, 0, CLAMP((fade_alpha * 255.0f), 0, 255), 0.0f, true, false);
}

void j1Transition::Scrolling()
{
	if (scroll_transition_texture != nullptr) {
		SDL_Rect r = dst_rect;
		r.x = 0;
		r.y = 0;
		App->render->Blit(scroll_transition_texture, dst_rect.x, dst_rect.y, nullptr, 0.0f);
	}
	switch (scroll_state)
	{
	case j1Transition::SNAPSHOT:
		if (cb != nullptr) {
			cb(0);
			cb = nullptr;
		}
		scroll_transition_texture = App->render->Snapshot();
		scroll_state = SCROLLING;
		break;
	case j1Transition::SCROLLING:
		if (dst_rect.x > App->win->screen_surface->w || dst_rect.x < -App->win->screen_surface->w) {
			completed = true;
			scroll_state = NOT_SCROLLING;
			SDL_DestroyTexture(scroll_transition_texture);
			scroll_transition_texture = nullptr;
		}
		break;
	default:
		break;
	}
}

void j1Transition::LoadScreen()
{
}

bool j1Transition::Awake(pugi::xml_node &)
{
	return true;
}

bool j1Transition::Start()
{
	completed = false;
	uint w = 0, h = 0;
	App->win->GetWindowSize(w, h);
	//float scale = App->win->GetScale();
	//w /= scale;
	//h /= scale;
	fade_alpha = 0;
	speed = 0;
	dst_rect = { 0, 0, (int)w, (int)h };
	int ret = SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	if (ret != 0)
	{
		LOG("Could not create transition surface: %s", SDL_GetError());
		return false;
	}
	return true;
}

bool j1Transition::PreUpdate()
{
	return true;
}

bool j1Transition::Update(float dt)
{
	bool ret = true;

	transcurred_time += dt;

	if (!completed)
	{
		switch (type)
		{
		case j1Transition::NONE:
			completed = true;
			if (cb != nullptr)
				cb(0);
			else ret = false;
			break;
		case j1Transition::FADE_TO_BLACK:
			Fade_To_Black();
			break;
		case j1Transition::SCROLL_RIGHT:
		case j1Transition::SCROLL_LEFT:
			dst_rect.x += speed * dt;
			Scrolling();
			break;
		case j1Transition::LOADING_SCREEN:
			break;
		default:
			break;
		}
	}

	return ret;
}

bool j1Transition::PostUpdate()
{
	return true;
}

bool j1Transition::CleanUp()
{
	return true;
}

bool j1Transition::Load(pugi::xml_node &)
{
	return true;
}

bool j1Transition::Save(pugi::xml_node &) const
{
	return true;
}
