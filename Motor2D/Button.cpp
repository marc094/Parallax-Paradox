#include "Button.h"
#include "j1Fonts.h"
#include "InterfaceElement.h"
#include "j1Input.h"


Button::Button(uint _x, uint _y, SDL_Texture* _tex, bool _enabled, SDL_Rect* _anim, Callback_v callback, SDL_Rect* _hovered_anim, SDL_Rect* _pressed_anim)
	: Sprite(_x, _y, _tex, _enabled, _anim)
{
	type = BUTTON;

	if (_hovered_anim == nullptr)
		hovered_anim = *_anim;
	else
		hovered_anim = *_hovered_anim;

	if (_pressed_anim == nullptr)
		pressed_anim = *_anim;
	else
		pressed_anim = *_pressed_anim;

	current_anim = &idle_anim;

	OnClick = callback;
}


Button::~Button()
{
}

void Button::OnHover()
{
}

bool Button::PostUpdate()
{
	if (label == nullptr) //Auto set label
	{
		for (p2List_item<InterfaceElement*>* current_element = elements.start;
			current_element != nullptr;
			current_element = current_element->next)
		{
			if (current_element->data->type == LABEL)
				label = (Label*)current_element->data;
		}
	}

	ComputeAbsolutePos();
	rect.x = (-anchor_point.x * rect.w) + abs_pos.x;
	rect.y = (-anchor_point.y * rect.h) + abs_pos.y;

	SDL_Rect Mouse;
	App->input->GetMousePosition(Mouse.x, Mouse.y);
	Mouse.w = CURSOR_WIDTH;
	Mouse.h = CURSOR_WIDTH;

	SDL_Rect result, r;
	r = rect;
	if (SDL_IntersectRect(&r, &Mouse, &result) == SDL_TRUE)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			current_anim = &pressed_anim;
			SetFocus();

			if (label != nullptr)
				label->setString("Left mouse button click");
			OnClick();
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			current_anim = &pressed_anim;

			if (label != nullptr)
				label->setString("Right mouse button click");
			OnClick();
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_IDLE && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_IDLE)
		{
			current_anim = &hovered_anim;

			if (label != nullptr)
				label->setColor({ 228, 204, 42, 255 });
			OnHover();
		}
	}
	else
	{
		current_anim = &idle_anim;
		SDL_Color curr;
		label->getColor(&curr);
		if (label != nullptr && curr.r != 255 || curr.g != 255 || curr.b != 255|| curr.a != 255)
			label->setColor({ 255, 255, 255, 255 });
	}

	/*if (in_focus)
	{
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			OnClick("focus");
			current_anim = &pressed_anim;
		}*/

		App->render->BlitGui(tex, rect.x, rect.y, current_anim);
	//}

	bool ret = InterfaceElement::PostUpdate();
	
	return ret;
}

void Button::setLabel(Label * label)
{
	this->label = label;
}
