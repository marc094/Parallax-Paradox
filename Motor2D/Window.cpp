
#include "Window.h"
#include "j1Input.h"
#include "j1Window.h"


Window::Window(uint _x, uint _y, SDL_Texture* _tex, bool _enabled, SDL_Rect* _anim) : Sprite(_x,_y, _tex, _enabled, _anim)
{
	type = Interfacetype::WINDOW;
	culled = false;
}


Window::~Window()
{
	if (in_focus)
		App->gui->setFocus(parent);
}

bool Window::PreUpdate()
{
	ComputeRects();

	App->input->GetMousePosition(Mouse.x, Mouse.y);
	Mouse.w = CURSOR_WIDTH;
	Mouse.h = CURSOR_WIDTH;

	//Focus();

	SDL_Rect result, r;
	r = (parent == nullptr) ? rect : result_rect;
	if (SDL_IntersectRect(&r, &Mouse, &result) == SDL_TRUE)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			App->gui->setFocus(this);
			iPoint m = { Mouse.x, Mouse.y };
			delta_pos_mouse = abs_pos - m;
		}
	}

	for (p2List_item<InterfaceElement*>* current_element = elements.start;
		current_element != nullptr;
		current_element = current_element->next)
	{
		current_element->data->PreUpdate();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && in_focus == true/*prev_mouse.x != Mouse.x && prev_mouse.y != Mouse.y*/)
	{
		DragWindow();
	}
	/*else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && in_focus == true)
		App->gui->setFocus(nullptr);*/

	return true;
}

void Window::DragWindow()
{
	rel_pos.x = Mouse.x + delta_pos_mouse.x - (abs_pos.x - rel_pos.x);
	rel_pos.y = Mouse.y + delta_pos_mouse.y - (abs_pos.y - rel_pos.y);
}

void Window::Focus()
{
	p2List_item<InterfaceElement*>* curr = elements.start;

	while(curr != NULL)
	{
		if (curr->data->in_focus)
		{
			onfocus = curr->data;
			if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			{
				onfocus->in_focus = false;
				if (curr->next == NULL)
					onfocus = elements.start->data;
				
				else
					onfocus = curr->next->data;

				onfocus->in_focus = true;
			}
			else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			{
				onfocus->in_focus = false;
				if (curr->prev == NULL)
					onfocus = elements.end->data;

				else
					onfocus = curr->prev->data;

				onfocus->in_focus = true;

				break;
			}
			
			break;
		}
			
		curr = curr->next;
	}
	
}

