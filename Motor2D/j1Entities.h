#ifndef __j1ENTITIES_H__
#define __j1ENTITIES_H__

#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "j1Map.h"
#include "Rect.h"
#include "Entity.h"
#include "BaseEnemy.h"
#include "Player.h"

class j1Entities :
	public j1Module
{
public:


	j1Entities();
	~j1Entities();


	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);


	// Called before the first frame
	virtual bool Start();


	// Called each loop iteration
	virtual bool PreUpdate();


	// Called each loop iteration
	virtual bool Update(float dt);


	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp();


	virtual bool Load(pugi::xml_node&);


	virtual bool Save(pugi::xml_node&) const;
	
	

	void Add_Enemy(BaseEnemy::Type type, fPoint position, LayerID layer);
	void Move(fPoint& position, fPoint& speed_vector) const;
	void j1Entities::Accelerate(fPoint& speed_vector, float x, float y) const;

	p2List<BaseEnemy*> Enemies;
	Player player;
	bool on_collision;

	SDL_Texture* texture = nullptr;
	Animation exclamation;
	Animation larva_cube;
	Animation air_cube;
	pugi::xml_node player_node;


private:
	pugi::xml_document animations;
	pugi::xml_node ground_enemy_node;
	pugi::xml_node flying_enemy_node;
	pugi::xml_node boxer_enemy_node;
	pugi::xml_node larva_enemy_node;
	pugi::xml_node air_enemy_node;


	float parallax_speed = 0.0f;

};

#endif