#include "Button.h"
#include "j1Fonts.h"
#include "InterfaceElement.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Scene.h"

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

	ComputeRects();
	rect.x = (int)(-anchor_point.x * rect.w) + abs_pos.x;
	rect.y = (int)(-anchor_point.y * rect.h) + abs_pos.y;

	SDL_Rect Mouse;
	App->input->GetMousePosition(Mouse.x, Mouse.y);
	Mouse.w = CURSOR_WIDTH;
	Mouse.h = CURSOR_WIDTH;

	/*int dx = 0, dy = 0, dw = 0, dh = 0;
	if (parent != nullptr) {
		//SDL_IntersectRect(&parent->content_rect, &rect, &result_rect);
		dx = result_rect.x - rect.x;
		dy = result_rect.y - rect.y;
		dw = result_rect.w - rect.w;
		dh = result_rect.h - rect.h;
	}*/

	SDL_Rect result;
	if (SDL_IntersectRect(&result_rect, &Mouse, &result) == SDL_TRUE)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			current_anim = &pressed_anim;
			SetFocus();
			if (OnClick != nullptr)
			{
				OnClick();

				if (type != SLIDER)
				App->audio->PlayFx(App->scene->button_sound);

			}

		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			current_anim = &pressed_anim;

			if (OnClick != nullptr)
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
		if (label != nullptr && (curr.r != 255 || curr.g != 255 || curr.b != 255|| curr.a != 255))
			label->setColor({ 255, 255, 255, 255 });
	}

	/*if (in_focus)
	{
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			OnClick("focus");
			current_anim = &pressed_anim;
		}*/

	/*SDL_Rect actual_anim_rect = *current_anim;
	actual_anim_rect.w += dw;
	actual_anim_rect.h += dh;
	actual_anim_rect.x += dx;
	actual_anim_rect.y += dy;

	App->render->BlitGui(tex, result_rect.x, result_rect.y, &actual_anim_rect);*/
	//}

	bool ret = InterfaceElement::PostUpdate();
	
	return ret;
}

void Button::setLabel(Label * label)
{
	this->label = label;
}
