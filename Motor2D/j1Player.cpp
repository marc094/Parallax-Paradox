#include "j1Player.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Map.h"
#include "p2List.h"
#include "j1Render.h"
#include <cstring>
#include "j1Textures.h"
#include "j1Window.h"


j1Player::j1Player()
{
	name.create("player");
}


j1Player::~j1Player()
{
}

void j1Player::Init()
{
	active = true;
}

// Called before render is available
bool j1Player::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	LOG("Loading Player Module");

	pugi::xml_parse_result result = player_anims.load_file(conf.child("animations").child_value());
	pugi::xml_node doc_node = player_anims.first_child().child("animationInfo");

	player_texture = App->tex->Load(player_anims.first_child().child("spritesheet").attribute("path").as_string());


	speed_vector = { 0, 0 };
	for (pugi::xml_node animation : doc_node.children()) {
		
		Animation* aux = new Animation();

		aux->name = animation.name();
		aux->speed = animation.attribute("duration").as_float();

		if (!strcmp(aux->name, "idle"))
			idle = aux;
		else if (!strcmp(aux->name, "jumping"))
			jumping = aux;
		else if (!strcmp(aux->name, "falling"))
			falling = aux;
		else if (!strcmp(aux->name, "landing"))
			landing = aux;
		else if (!strcmp(aux->name, "walking"))
			walking = aux;
		else if (!strcmp(aux->name, "changing"))
			changing_layers = aux;



		for (pugi::xml_node frames : animation.children())
		{
			SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("h").as_int(), frames.attribute("h").as_int() };
			aux->PushBack(aux_rect);
			animation_list.add(aux);
		}
	}
	current_animation = idle;

	if (result == NULL) {
		LOG("Could not load map xml file %s. pugi error: %s", conf.child("animations").child_value(), result.description());
		ret = false;
	}
	return true;
}

// Called before the first frame
bool j1Player::Start()
{
	player_texture = App->tex->Load(player_anims.first_child().child("spritesheet").attribute("path").as_string());
	
	position = App->map->GetInitialPlayerPos();

	scale = App->win->GetScale();

	current_animation = idle;
	return true;
}

// Called each loop iteration
bool j1Player::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Player::Update(float dt)
{

	return true;
}

// Called each loop iteration
bool j1Player::PostUpdate()
{

	SelectAnim(speed_vector);
	AnimationFrame frame = current_animation->GetCurrentFrame();


	Move();


	Accelerate(0, 1);


	App->render->camera.x = -position.x + App->render->camera.w / 2;
	App->render->camera.y = -position.y + App->render->camera.h / 2;



	if (flipped == true)
		App->render->Blit(player_texture, position.x, position.y, &frame.rect, 1.0f, 0, 0, 0, false, true);
	else
		App->render->Blit(player_texture, position.x, position.y, &frame.rect, 1.0f, 0, 0, 0, false);
	return true;
}

// Called before quitting
bool j1Player::CleanUp()
{
	return true;
}

bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();

	return true;
}

bool j1Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void j1Player::Move() {


	Checkcollisions();

	position.x += speed_vector.x;
	position.y += speed_vector.y;


	speed_vector.x = REDUCE_TO(speed_vector.x, 0, DECELERATION * 2);
	speed_vector.y = REDUCE_TO(speed_vector.y, 0, DECELERATION);
	

	state = IDLE;
}


void j1Player::Accelerate(int x, int y) {
	speed_vector.x += ((float)x) / (1.0f/ACCELERATION);
	speed_vector.y += ((float)y) / (1.0f/ACCELERATION);

	speed_vector.x = CLAMP(speed_vector.x, -5, 5);
	speed_vector.y = CLAMP(speed_vector.y, -5, 5);

	state = RUNNING;
}


void j1Player::SelectAnim(fPoint speed_vect)
{
	if (speed_vect.x != 0)
	{ 
		current_animation = walking;

		if (speed_vect.x > 0)
		{
			flipped = false;
			
		}
		else if (speed_vect.x < 0)
		{
			
			flipped = true;
		}
	}
	else
		current_animation = idle;
}

void j1Player::Checkcollisions()
{
	AnimationFrame frame = current_animation->GetCurrentFrame();

	if (App->map->data.layers[1] != NULL)
	{
		uint i = 0;
		while (i < App->map->data.layers[1]->layer_colliders.count() && App->map->data.layers[1]->layer_colliders[i] != NULL)
		{
			SDL_Rect aux = App->map->data.layers[1]->layer_colliders[i]->rect;
			SDL_Rect player_frame = frame.rect;
			App->render->DrawQuad(aux, 0, 255, 0, App->map->data.layers[1]->parallax_speed);
		
			aux.x = (int)( App->map->data.layers[1]->parallax_speed) + aux.x * scale;
			aux.y = (int)(App->map->data.layers[1]->parallax_speed) + aux.y * scale;
			aux.w *= scale;
			aux.h *= scale;
			player_frame.w *= scale;
			player_frame.h *= scale;
			

			if (position.x + player_frame.w + speed_vector.x > aux.x && position.x + speed_vector.x < aux.x + aux.w && position.y + player_frame.h +speed_vector.y > aux.y && position.y + speed_vector.y < aux.y + aux.h)
			{
				if (position.x < aux.x + aux.w && position.x + player_frame.w > aux.x)
				{
					if (position.y + speed_vector.y < aux.y + aux.h && position.y > aux.y + aux.h && speed_vector.y < 0)
					{
						speed_vector.y = 0;
					}
					else if (position.y + player_frame.h + speed_vector.y > aux.y && speed_vector.y >= 0)
					{
						speed_vector.y = 0;
					}
				}
				if (position.y < aux.y + aux.h && position.y + player_frame.h > aux.y)
				{
					if (position.x + speed_vector.x < aux.x + aux.w && position.x > aux.x + aux.w && speed_vector.x < 0)
					{
						speed_vector.x = 0;
					}
					else if (position.x + player_frame.w + speed_vector.x > aux.x && speed_vector.x >= 0)
					{
						speed_vector.x = 0;
					}
				}
			}
			i++;
		}
	}
}