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
	pugi::xml_document player_anims;
	pugi::xml_parse_result result = player_anims.load_file("animations.xml");
	pugi::xml_node doc_node = player_anims.first_child();


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
		else if (!strcmp(aux->name, "walking_right"))
			walking_right = aux;
		else if (!strcmp(aux->name, "walking_left"))
			walking_left = aux;
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
		LOG("Could not load map xml file %s. pugi error: %s", "animations.xml", result.description());
		ret = false;
	}
	return true;
}

// Called before the first frame
bool j1Player::Start()
{
	position = App->map->GetInitialPlayerPos();
	player_texture = App->tex->Load("Character sprites.png");

	return true;
}

// Called each loop iteration
bool j1Player::PreUpdate()
{

	App->render->camera.x = -position.x;
	App->render->camera.y = -position.y;
	return true;
}

// Called each loop iteration
bool j1Player::Update(float dt)
{

	SelectAnim(speed_vector);
	AnimationFrame frame = current_animation->GetCurrentFrame();

	speed_vector.x = 0;
	Move(0,0);

	
	App->render->Blit(player_texture, (-App->render->camera.x + (App->render->camera.w/2)), (-App->render->camera.y + (App->render->camera.h / 2)), &frame.rect,1.0f,0,0,0,false);
	return true;
}

// Called each loop iteration
bool j1Player::PostUpdate()
{
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

void j1Player::Move(int x, int y) {

	speed_vector.x = x;
	speed_vector.y += y;

	speed_vector.y += App->map->gravity;

	if (position.y + speed_vector.y > App->map->GetInitialPlayerPos().y + 80 )
	{

		speed_vector.y = 0;
	}


	position.x += speed_vector.x;
	position.y += speed_vector.y;
}


void j1Player::SelectAnim(fPoint speed_vect)
{
	if (speed_vect.x != 0)
	{
		if (speed_vect.x > 0)
		{
			current_animation = walking_right;
			
		}
		else if (speed_vect.x < 0)
		{
			
			current_animation = walking_left;
		}
	}
	else
	{
		current_animation = idle;
	}

}