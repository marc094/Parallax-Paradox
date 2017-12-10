#include "Sprite.h"
#include "j1Render.h"

Sprite::Sprite(uint _x, uint _y, SDL_Texture* _tex, bool _enabled, SDL_Rect* _anim) : InterfaceElement()
{
	type = SPRITE;
	tex = _tex;
	rel_pos.x = _x;
	rel_pos.y = _y;
	enabled = _enabled;

	if (_anim != NULL)
	{
		idle_anim = *_anim;
	}
	else
	{
		idle_anim.x = 0;
		idle_anim.y = 0;
		SDL_QueryTexture(tex, NULL, NULL, &idle_anim.w, &idle_anim.h);
	}

	rect.w = idle_anim.w;
	rect.h = idle_anim.h;

	content_rect = rect;

	current_anim = &idle_anim;
}


Sprite::~Sprite()
{
}

bool Sprite::PostUpdate()
{
	current_anim = &idle_anim;

	//if (in_focus) {
		ComputeAbsolutePos();
		rect.x = (-anchor_point.x * rect.w) + abs_pos.x;
		rect.y = (-anchor_point.y * rect.h) + abs_pos.y;
		App->render->BlitGui(tex, rect.x, rect.y, current_anim);
	//}

	bool ret = InterfaceElement::PostUpdate();
	return ret;
}
