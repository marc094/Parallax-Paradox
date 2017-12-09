#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "InterfaceElement.h"
#include "Label.h"

class InterfaceElement;
class Sprite;
class Label;
class Button;
class Window;

#define CURSOR_WIDTH 2

// TODO 1: Create your structure of classes

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	Sprite* AddSprite(int x, int y, SDL_Texture* tex, bool enabled = true, SDL_Rect* anim = NULL);
	Label* AddLabel(int x, int y, int psize, const char* font_path = nullptr, SDL_Color color = {0,0,0,255}, Label::RenderMode mode = Label::BLENDED, const char* format = nullptr, ...);
	Button* AddButton(int _x, int _y, SDL_Texture* _tex, bool _enabled, SDL_Rect* _anim, Callback_n callback, SDL_Rect* _hovered_anim = nullptr, SDL_Rect* _pressed_anim = nullptr);
	Window* AddWindow(int x, int y, SDL_Texture* tex, bool enabled = true, SDL_Rect* anim = NULL);
	const SDL_Texture* GetAtlas() const;
	p2SString atlas_file_name;

	uiPoint GetGuiSize();
	void setFocus(InterfaceElement* elem);
	InterfaceElement* getFocusedItem();

	InterfaceElement* AddElement(InterfaceElement* elem);
	bool RemoveElement(InterfaceElement* elem);

	bool debug_draw = false;
	p2List<InterfaceElement*> elements;

private:

	uiPoint gui_size;
	float scale;

	InterfaceElement* focused_item = nullptr;
};

#endif // __j1GUI_H__