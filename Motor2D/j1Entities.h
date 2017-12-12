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
#include "Coin.h"

class j1Entities :
	public j1Module
{
public:
	j1Entities();
	~j1Entities();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool Update(float dt) override;

	bool PostUpdate() override;
	// Called before quitting
	bool CleanUp() override;

	bool Load(pugi::xml_node&) override;

	bool Save(pugi::xml_node&) const override;

	BaseEnemy* Add_Enemy(BaseEnemy::Type type, fPoint position, LayerID layer);
	Coin* Add_Coin(fPoint pos);

	p2List<BaseEnemy*> Enemies;
	p2List<Coin*> Coins;
	Player player;

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
};
#endif