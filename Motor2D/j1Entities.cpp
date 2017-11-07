#include "j1Entities.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"

j1Entities::j1Entities() : j1Module()
{
	name.create("entities");
}


j1Entities::~j1Entities()
{
}

bool j1Entities::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	LOG("Loading Module Entities");

	pugi::xml_parse_result result = enemy_animations.load_file(conf.child("animations").child_value());

	if (result == NULL) {
		LOG("Could not load map xml file %s. pugi error: %s", conf.child("animations").child_value(), result.description());
		ret = false;
	}



	pugi::xml_node doc_node = enemy_animations.child("animations");
	ground_enemy_node = doc_node.child("Enemies").child("ground");
	

	


	return true;
}
bool j1Entities::Start()
{	
	enemy_texture = App->tex->Load(enemy_animations.first_child().child("spritesheet").attribute("path").as_string());
	
	Add_Enemy(GROUND, { 1000,1005 }, COLLIDER_FRONT_LAYER);

	return true;
}

bool j1Entities::Update(float dt)
{

	p2List_item<Enemy*>* current_enemy = Enemies.start;
	while (!current_enemy == NULL)
	{
		//Check Collisions
		SDL_Rect collider_rect = current_enemy->data->current_animation->GetCurrentFrame().rect;
		collider_rect.x = current_enemy->data->position.x;
		collider_rect.y = current_enemy->data->position.y;
		bool on_collision = App->collision->CheckEnemyCollisions(collider_rect, App->player->player_rect);
		
		App->render->Blit(enemy_texture, current_enemy->data->position.x, current_enemy->data->position.y, &current_enemy->data->current_animation->GetCurrentFrame().rect);
		current_enemy = current_enemy->next;
	}
	return true;
}


void j1Entities::Add_Enemy(Type type, fPoint position, ColliderType layer)
{
	Enemy* aux = new Enemy();
	aux->position = position;
	aux->state = IDLE;
	aux->speed_vect = { 0,0 };
	aux->currentLayer = layer;

	if (type == GROUND)
	{
		for (pugi::xml_node animation : ground_enemy_node.child("animationInfo").children()) {

			Animation aux_anim;

			aux_anim.name = animation.name();
			aux_anim.speed = animation.attribute("duration").as_float();
			aux_anim.loop = animation.attribute("loop").as_bool();

			for (pugi::xml_node frames : animation.children())
			{
				SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("h").as_int(), frames.attribute("h").as_int() };
				aux_anim.PushBack(aux_rect);
			}

			if (!strcmp(aux_anim.name, "idle"))
				aux->idle_anim = aux_anim;
			else if (!strcmp(aux_anim.name, "walking"))
				aux->moving_anim = aux_anim;
		}

	}

	aux->gravity = ground_enemy_node.child("enemyInfo").attribute("gravity").as_bool();
	aux->current_animation = &aux->idle_anim;
	aux->collider = aux->current_animation->GetCurrentFrame().rect;

	Enemies.add(aux);
}