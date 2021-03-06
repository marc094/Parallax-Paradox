#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Window.h"
#include "SDL/include/SDL.h"
#include "SDL\include\SDL_gamecontroller.h"


j1Input::j1Input() : j1Module()
{
	name.create("input");

	keyboard = new keyEvent[MAX_KEYS];
	memset(keyboard, { KEY_IDLE }, sizeof(keyEvent) * MAX_KEYS);
	memset(mouse_buttons, { KEY_IDLE }, sizeof(keyEvent) * NUM_MOUSE_BUTTONS);

	//init controller variables
	controller.buttons = new keyEvent[SDL_CONTROLLER_BUTTON_MAX];
	controller.axis = new double[SDL_CONTROLLER_AXIS_MAX];
	memset(controller.buttons, { KEY_IDLE }, sizeof(keyEvent) * SDL_CONTROLLER_BUTTON_MAX);
	memset(controller.axis, 0, sizeof(double) * SDL_CONTROLLER_AXIS_MAX);
}

// Destructor
j1Input::~j1Input()
{
	delete[] keyboard;
}

// Called before render is available
bool j1Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool j1Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool j1Input::PreUpdate()
{
	static SDL_Event event;
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	mouse_x = mouse_x_prev;
	mouse_y = mouse_y_prev;

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			keyboard[i].blocked = false;
			if(keyboard[i].keyState == KEY_IDLE)
				keyboard[i].keyState = KEY_DOWN;
			else
				keyboard[i].keyState = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i].keyState == KEY_REPEAT || keyboard[i].keyState == KEY_DOWN)
				keyboard[i].keyState = KEY_UP;
			else
				keyboard[i].keyState = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		mouse_buttons[i].blocked = false;
		if(mouse_buttons[i].keyState == KEY_DOWN)
			mouse_buttons[i].keyState = KEY_REPEAT;

		if(mouse_buttons[i].keyState == KEY_UP)
			mouse_buttons[i].keyState = KEY_IDLE;
	}

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
		if (SDL_GameControllerGetButton(controller._sdl_ref, (SDL_GameControllerButton)i) == 1)
		{
			controller.buttons[i].blocked = false;
			if (controller.buttons[i].keyState == KEY_IDLE)
				controller.buttons[i].keyState = KEY_DOWN;
			else
				controller.buttons[i].keyState = KEY_REPEAT;
		}
		else
		{
			if (controller.buttons[i].keyState == KEY_REPEAT || controller.buttons[i].keyState == KEY_DOWN)
				controller.buttons[i].keyState = KEY_UP;
			else
				controller.buttons[i].keyState = KEY_IDLE;
		}
	}

	for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++) {
		Sint16 axis = SDL_GameControllerGetAxis(controller._sdl_ref, (SDL_GameControllerAxis)i);
		double axis_normalised = (double)axis / INT16_MAX;
		controller.axis[i] = axis_normalised;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1].keyState = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1].keyState = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_CONTROLLERDEVICEREMOVED:
				if (SDL_NumJoysticks() == 0)
					controller.connected = false;
				break;

			case SDL_CONTROLLERDEVICEADDED:
				CheckControllers();
				break;

			case SDL_MOUSEMOTION:
				float scale = (int)App->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = mouse_x_prev = event.motion.x / scale;
				mouse_y = mouse_y_prev = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
		}
	}

	return true;
}

// Called before quitting
bool j1Input::CleanUp(pugi::xml_node&)
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool j1Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void j1Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void j1Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

bool j1Input::CheckControllers()
{
	bool ret = false;

	if (controller.connected)
		return true;

	int numSticks = SDL_NumJoysticks();
	if (numSticks > 0)
	{
		controller._sdl_ref = SDL_GameControllerOpen(0);
		if (controller._sdl_ref == nullptr)
		{
			LOG("Controller couldn't be initialized SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			ret = true;
			controller.connected = true;
		}
	}
	return ret;
}