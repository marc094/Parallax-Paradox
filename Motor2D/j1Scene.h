#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"


struct SDL_Texture;

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

	bool ret = true;

private:
	uint level;
	uint max_level;
	p2DynArray<p2SString> xml_file_name;
	bool playing;
	SDL_Texture* menu_background = nullptr;
	SDL_Texture* title = nullptr;
	SDL_Texture* buttons = nullptr;


};

void button_callback(const char*);
void Game_start();
void Game_exit();
#endif // __j1SCENE_H__