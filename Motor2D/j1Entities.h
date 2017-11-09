#ifndef __j1ENTITIES_H__
#define __j1ENTITIES_H__

#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "j1Map.h"


class j1Entities :
	public j1Module
{
public:

	struct Enemy
	{
		enum Type
		{
			GROUND,
			BOXER,
			AIR,
		};

		enum State
		{
			IDLE,
			ALERT,
		};

		Animation* current_animation = nullptr;
		Animation idle_anim;
		Animation moving_anim;
		Animation alert_anim;
		iRect collider;
		fPoint position;
		ColliderType currentLayer;
		SDL_Rect enemyrect;
		State state;
		Type type;
		fPoint speed_vect;
		bool gravity;
	};


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
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt);


	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp();


	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	void Add_Enemy(Enemy::Type type, fPoint position, ColliderType layer);
	void Move(fPoint& position, fPoint& speed_vector) const;
	void j1Entities::Accelerate(fPoint& speed_vector, float x, float y) const;

	p2List<Enemy*> Enemies;
	bool on_collision;


private:
	pugi::xml_document enemy_animations;
	pugi::xml_node ground_enemy_node;
	pugi::xml_node flying_enemy_node;
	pugi::xml_node boxer_enemy_node;
	SDL_Texture* enemy_texture = nullptr;
	Animation exclamation;
	float parallax_speed = 0.0f;

};

#endif