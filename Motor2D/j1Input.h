#ifndef __j1INPUT_H__
#define __j1INPUT_H__

#include "j1Module.h"

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum j1KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

typedef struct {
	j1KeyState keyState = KEY_IDLE;
	bool blocked = false;
} keyEvent;

class j1Input : public j1Module
{

public:

	j1Input();

	// Destructor
	virtual ~j1Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	j1KeyState GetKey(int id) const
	{
		if (keyboard[id].blocked)
			return KEY_IDLE;
		return keyboard[id].keyState;
	}

	j1KeyState GetMouseButtonDown(int id) const
	{
		if (mouse_buttons[id - 1].blocked)
			return KEY_IDLE;
		return mouse_buttons[id - 1].keyState;
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

	bool BlockMouseEvent(int event_id) {
		if (mouse_buttons[event_id - 1].blocked)
			return false;
		mouse_buttons[event_id - 1].blocked = true;
		return true;
	}

	bool BlockKeyboardEvent(int event_id) {
		if (keyboard[event_id].blocked)
			return false;
		keyboard[event_id].blocked = true;
		return true;
	}

	void BlockMouse() {
			mouse_x = mouse_y = INT_MAX - 2;
	}

private:
	bool		windowEvents[WE_COUNT];
	keyEvent*	keyboard;
	keyEvent	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x;
	int			mouse_motion_y;
	int			mouse_x, mouse_x_prev;
	int			mouse_y, mouse_y_prev;
};

#endif // __j1INPUT_H__