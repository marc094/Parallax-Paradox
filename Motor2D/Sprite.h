#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "InterfaceElement.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"


class Sprite :
	public InterfaceElement
{
public:
	Sprite(uint _x = 0, uint _y = 0, SDL_Texture* _tex = nullptr, SDL_Rect _anim = { 0, 0, 0, 0 }, bool _enabled = false);
	~Sprite();
	bool Update(float dt) override;

	SDL_Rect idle_anim;
};

#endif