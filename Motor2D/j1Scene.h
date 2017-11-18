#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

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

	void CheckEnd();

private:
	uint level;
	uint max_level;
	uint current_song;
	uint level1_song;
	uint level2_song;
	p2DynArray<p2SString> xml_file_name;
	bool playing;
};

#endif // __j1SCENE_H__