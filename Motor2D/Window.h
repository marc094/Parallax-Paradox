#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Sprite.h"
#include "Button.h"
#include "Label.h"

class Window :
	public Sprite
{
public:
	Window(uint _x = 0, uint _y = 0, SDL_Texture* _tex = nullptr, bool _enabled = false, SDL_Rect* _anim = nullptr);
	~Window();

	void DragWindow();
	bool PreUpdate() override;

	void Focus();

	
	InterfaceElement* onfocus = nullptr;

	SDL_Rect Mouse;
	bool locked = false;
	iPoint delta_pos_mouse;
};
#endif