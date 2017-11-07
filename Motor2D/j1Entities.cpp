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
	

	exclamation.PushBack({ 0,36,3,8 });


	return true;
}
bool j1Entities::Start()
{	
	enemy_texture = App->tex->Load(enemy_animations.first_child().child("spritesheet").attribute("path").as_string());
	
	Add_Enemy(GROUND, { 1000,1005 }, COLLIDER_FRONT_LAYER);

	on_collision = false;
	return true;
}

bool j1Entities::Update(float dt)
{

	p2List_item<Enemy*>* current_enemy = Enemies.start;
	while (current_enemy != NULL)
	{
		//Check Collisions 

		Rect collider_rect = current_enemy->data->current_animation->GetCurrentFrame().rect;
		collider_rect.x = current_enemy->data->position.x;
		collider_rect.y = current_enemy->data->position.y;
		Rect player_rect = App->player->player_rect;
		player_rect.x = App->player->GetPosition().x;
		player_rect.y = App->player->GetPosition().y;


		if (App->collision->CheckEnemyCollisions(collider_rect, player_rect))
			App->Reload();

		

		//Move
		Rect alert_rect;
		alert_rect.x = collider_rect.x - 50;
		alert_rect.y = collider_rect.y - 50;
		alert_rect.w = collider_rect.w + 100;
		alert_rect.h = collider_rect.h + 100;

		bool flipped = false;

		if (App->collision->CheckEnemyCollisions(alert_rect, player_rect))
		{
			current_enemy->data->state = ALERT;
		}
	

		if (current_enemy->data->state == ALERT)
		{
			current_enemy->data->current_animation = &current_enemy->data->alert_anim;
			App->render->Blit(enemy_texture, collider_rect.x + ((collider_rect.w - exclamation.GetCurrentFrame().rect.w) / 2) , collider_rect.y - 10, &exclamation.GetCurrentFrame().rect.toSDL());

			if (player_rect.x < collider_rect.x)
			{
				flipped = true;
			}

			if (current_enemy->data->current_animation->Finished())
			{
				current_enemy->data->state = IDLE;
			}
		}
		else
		{
			current_enemy->data->alert_anim.Reset();
			current_enemy->data->state = IDLE;
			current_enemy->data->current_animation = &current_enemy->data->idle_anim;
			Accelerate(&current_enemy->data->speed_vect, 0.6f, 0);
		}
		
		App->collision->Checkcollisions(current_enemy->data->currentLayer, collider_rect, current_enemy->data->position, &current_enemy->data->speed_vect);

		Move(&current_enemy->data->position, &current_enemy->data->speed_vect);


		//Gravity
		Accelerate(&current_enemy->data->speed_vect, 0, 0.5f);
		

		//Blit

		App->render->Blit(enemy_texture, current_enemy->data->position.x, current_enemy->data->position.y, &current_enemy->data->current_animation->GetCurrentFrame().rect.toSDL(), 1.0f, 0, 0, 0, true, flipped);
		current_enemy = current_enemy->next;
	}
	return true;
}
bool j1Entities::CleanUp()
{
	App->tex->UnLoad(enemy_texture);
	enemy_texture = nullptr;
	Enemies.clear();

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
			else if (!strcmp(aux_anim.name, "alert"))
				aux->alert_anim = aux_anim;
		}

	}

	aux->gravity = ground_enemy_node.child("enemyInfo").attribute("gravity").as_bool();
	aux->current_animation = &aux->idle_anim;
	aux->collider = aux->current_animation->GetCurrentFrame().rect;

	Enemies.add(aux);
}

void j1Entities::Move(fPoint* position, fPoint* speed_vector) {


	position->x += speed_vector->x;
	position->y += speed_vector->y;


	speed_vector->x = REDUCE_TO(speed_vector->x, 0, DECELERATION * 2);
	//speed_vector.y = REDUCE_TO(speed_vector.y, 0, DECELERATION);


}

void j1Entities::Accelerate(fPoint* speed_vector, float x, float y) {
	speed_vector->x += (x) / (1.0f / ACCELERATION);
	speed_vector->y += (y) / (1.0f / ACCELERATION);

	//speed_vector->x = CLAMP(speed_vector.x, -5, 5);
	//speed_vector->y = CLAMP(speed_vector.y, -10, 10);

}