#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Sprite.h"
#include "Label.h"
#include "j1Gui.h"

class Button :
	public Sprite
{
public:
	Button(uint _x, uint _y, SDL_Texture* _tex, bool _enabled, SDL_Rect* _anim, Callback_c callback, SDL_Rect* _hovered_anim = nullptr, SDL_Rect* _pressed_anim = nullptr);
	~Button();
	
	enum STATE {
		IDLE,
		PRESSED,
		HOVERED,
		DISABLED
	} state;
	
	void OnHover();
	Callback_c OnClick = nullptr;
	bool PostUpdate() override;
	void setLabel(Label* label);

private:
	Label* label = nullptr;
	SDL_Rect pressed_anim;
	SDL_Rect hovered_anim;
	SDL_Rect disabled_anim;
	p2SString idle_string;
	p2SString pressed_string;
	p2SString hovered_string;
	p2SString disabled_string;

};

#endif // !__BUTTON_H__