#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"


struct SDL_Texture;
class Window;
class Button;
class Sprite;
class Label;
class Slider;

class j1Scene : public j1Module
{
public:

	j1Scene();

	enum state {
		IN_GAME,
		IN_MENU,
		TRANSITION
	}state = IN_MENU;

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ChangeScene(uint level);
	
	void CheckInput(float dt);

	void CheckEnd();

	bool Load(pugi::xml_node& data);

	bool Save(pugi::xml_node& data) const;

	bool ret = true;

	void ChangeLifes();
	void SumCoin();
	Window* credits_window = nullptr;
	Window* settings_window = nullptr;
	Slider* credits_slider;
	Sprite* title_spr2;
	bool credits_bool = false;

	uint jump_sound = 0;
	uint hit_sound = 0;
	uint change_sound = 0;
	uint level_sound = 0;

private:
	uint level;
	uint max_level;
	p2DynArray<p2SString> xml_file_name;
	bool playing;
	SDL_Texture* menu_background = nullptr;
	SDL_Texture* title = nullptr;
	SDL_Texture* buttons = nullptr;
	SDL_Texture* credits_win = nullptr;
	SDL_Texture* settings_win = nullptr;
	p2List<Button*> menu_buttons;
	p2List<Sprite*> lives;
	SDL_Texture* lifes_sprite;
	j1Timer time;
	Label* time_lab;
	Label* coin_lab;

};

void button_callback(const char*);
void Game_start();
void Game_continue();
void Show_Credits();
void Hide_Credits();
void Drag_Credits();
#endif // __j1SCENE_H__