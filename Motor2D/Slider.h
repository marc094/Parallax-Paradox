#pragma once
#include "Button.h"
#include "Rect.h"
class Slider :
	public Button
{
public:
	Slider(uint _x = 0, uint _y = 0, SDL_Texture* _tex = nullptr, bool _enabled = false, SDL_Rect* _anim = nullptr, Callback_v callback = nullptr, SDL_Rect* _hovered_anim = nullptr, SDL_Rect* _pressed_anim = nullptr, bool _axis = 1, InterfaceElement* parent = nullptr);
	~Slider();
	iRect upper_rect;

	void DragSlider();
	bool PreUpdate() override;

	void Focus();


	InterfaceElement* onfocus = nullptr;
	bool axis;
	SDL_Rect Mouse;
	bool locked = false;
	iPoint delta_pos_mouse;

};

