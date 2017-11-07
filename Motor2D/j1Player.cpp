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
#include "j1Scene.h"
#include "j1Collision.h"


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


	if (result == NULL) {
		LOG("Could not load map xml file %s. pugi error: %s", conf.child("animations").child_value(), result.description());
		ret = false;
	}

	pugi::xml_node doc_node = player_anims.child("animations").child("player").child("animationInfo");

	speed_vector = { 0, 0 };

	for (pugi::xml_node animation : doc_node.children()) {
		
		Animation* aux = new Animation();

		aux->name = animation.name();
		aux->speed = animation.attribute("duration").as_float();
		aux->loop = animation.attribute("loop").as_bool();

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

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{	
	player_texture = App->tex->Load(player_anims.first_child().child("spritesheet").attribute("path").as_string());

	position = App->map->GetInitialPlayerPos();

	scale = App->win->GetScale();

	current_layer = COLLIDER_FRONT_LAYER;

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


	App->collision->Checkcollisions(current_layer, frame.rect,position,&speed_vector);
	if (speed_vector.y == 0)
	{
			speed_vector.y = 0;
			isjumping = false;
			jumping->Reset();
	}

	Move();


	//Gravity
	Accelerate(0, 0.5f);


	App->render->camera.x = -position.x *scale + App->render->camera.w / 2;
	App->render->camera.y = -position.y *scale + App->render->camera.h / 2;

	if (position.y > 1400)
		App->Reload();

	App->render->Blit(player_texture, position.x, position.y, &frame.rect, 1.0f, 0, 0, 0, true, flipped);

	return true;
}

// Called before quitting
bool j1Player::CleanUp()
{
	App->tex->UnLoad(player_texture);
	player_texture = nullptr;
	player_anims.reset();
	animation_list.clear();
	idle = nullptr;
	jumping = nullptr;
	falling = nullptr;
	landing = nullptr;
	walking = nullptr;
	changing_layers = nullptr;
	current_animation = nullptr;

	position = { 0.0f,0.0f };
	speed_vector = { 0.0f,0.0f };
	return true;
}

bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();
	speed_vector = { 0, 0 };
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


	position.x += speed_vector.x;
	position.y += speed_vector.y;


	speed_vector.x = REDUCE_TO(speed_vector.x, 0, DECELERATION * 2);
	//speed_vector.y = REDUCE_TO(speed_vector.y, 0, DECELERATION);
	

	state = IDLE;
}


void j1Player::Accelerate(float x, float y) {
	speed_vector.x += (x) / (1.0f/ACCELERATION);
	speed_vector.y += (y) / (1.0f/ACCELERATION);

	speed_vector.x = CLAMP(speed_vector.x, -5, 5);
	speed_vector.y = CLAMP(speed_vector.y, -10, 10);

	state = RUNNING;
}


void j1Player::SelectAnim(fPoint speed_vect)
{
	if (isjumping == true)
		current_animation = jumping;
	else if (speed_vect.x != 0)
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


void j1Player::SwapLayer() {
	if (current_layer == COLLIDER_BACK_LAYER) {
		current_layer = COLLIDER_FRONT_LAYER;
		//scale = 1.0f;
	}
	else {
		current_layer = COLLIDER_BACK_LAYER;
		//scale = 0.7f;
	}
}

uint j1Player::GetCurrentLayer() {
	return (uint)current_layer - 2;
}
