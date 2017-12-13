#ifndef __INTERFACE_ELEMENT_H__
#define __INTERFACE_ELEMENT_H__

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"

class InterfaceElement
{
public:
	InterfaceElement();
	~InterfaceElement();
	enum Interfacetype
	{
		NONE = -1,
		BUTTON,
		LABEL,
		SPRITE,
		TEXT_INPUT,
		CHECK_BUTTON,
		WINDOW
	};


	bool Enable(bool enable);
	bool isEnabled();
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual SDL_Rect getRect() const;
	virtual void SetContentRect(int x_margin = INT_MAX, int y_margin = INT_MAX, int x = 0, int y = 0);
	SDL_Rect GetContentRect() const;
	virtual int getPositionX() const;
	virtual int getPositionY() const;
	virtual void setPosition(int x, int y);
	virtual void setPositionX(int x);
	virtual void setPositionY(int y);
	virtual void setScale(float scale);
	virtual float getScale() const;
	Interfacetype getType() const;

	void SetAnchor(float x, float y);
	void GetAnchor(float& x, float& y) const;
	float GetAnchorX() const;
	float GetAnchorY() const;

	void SetParent(InterfaceElement* parent);

	void ComputeRects();

	SDL_Texture* tex = nullptr;
	Interfacetype type = NONE;
	SDL_Rect rect;
	SDL_Rect content_rect;
	SDL_Rect result_rect;
	float scale = 1.0f;
	bool enabled = true;
	iPoint rel_pos;
	iPoint abs_pos;
	iPoint initial_pos;
	bool in_focus = false;
	bool culled = true;

	void SetFocus();
	InterfaceElement* NextElement(InterfaceElement* curr_child);

protected:
	InterfaceElement* AddElement(InterfaceElement* elem);
	p2List<InterfaceElement*> elements;
	InterfaceElement* parent = nullptr;

	fPoint anchor_point = { 0.5f, 0.5f };
	SDL_Rect* current_anim = nullptr;
	const bool interactuable = false;
};
#endif