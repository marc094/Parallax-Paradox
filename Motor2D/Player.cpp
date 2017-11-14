#include "Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Entities.h"


Player::Player()
{
}

Player::~Player()
{
}

bool Player::Awake()
{
	bool ret = true;


	pugi::xml_node doc_node = App->entities->player_node.child("animationInfo");

	speed_vect = { 0, 0 };

	for (pugi::xml_node animation : doc_node.children()) {

		Animation aux/* = new Animation()*/;

		aux.name = animation.name();
		aux.speed = animation.attribute("duration").as_float();
		aux.loop = animation.attribute("loop").as_bool();

		for (pugi::xml_node frames : animation.children())
		{
			SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("w").as_int(), frames.attribute("h").as_int() };
			aux.PushBack(aux_rect);
			animation_list.add(aux);
		}

		if (!strcmp(aux.name, "idle"))
			idle_anim = aux;
		else if (!strcmp(aux.name, "jumping"))
			jumping_anim = aux;
		else if (!strcmp(aux.name, "falling"))
			falling_anim = aux;
		else if (!strcmp(aux.name, "landing"))
			landing_anim = aux;
		else if (!strcmp(aux.name, "walking"))
			moving_anim = aux;
		else if (!strcmp(aux.name, "changing"))
			changing_layers_anim = aux;
	}

	return ret;
}

bool Player::Start()
{

	position = App->map->GetInitialPlayerPos();


	current_layer = FRONT_LAYER;

	current_animation = &idle_anim;

	collider = current_animation->GetCurrentFrame().rect;

	return true;
}

bool Player::PreUpdate()
{
	AnimationFrame frame = current_animation->GetCurrentFrame();

	return true;
}

bool Player::Update(float dt)
{
	SelectAnim(speed_vect);


	App->collision->Checkcollisions(current_layer, collider, position, &speed_vect);
	if (speed_vect.y == 0)
	{
		speed_vect.y = 0;
		isjumping = false;
		jumping_anim.Reset();
	}

	Move();


	//Gravity
	Accelerate(0, 0.5f);


	App->render->camera.x = -position.x *scale + App->render->camera.w / 2;
	App->render->camera.y = -position.y *scale + App->render->camera.h / 2;

	if (position.y > 1400)
		App->Reload();

	App->render->Blit(App->entities->texture, position.x, position.y, &current_animation->GetCurrentFrame().rect.toSDL_Rect(), 1.0f, 0, 0, 0, true, flipped);
	return true;
}

bool Player::CleanUp()
{

	animation_list.clear();
	idle_anim.Reset();
	jumping_anim.Reset();
	falling_anim.Reset();
	landing_anim.Reset();
	moving_anim.Reset();
	changing_layers_anim.Reset();
	current_animation = nullptr;

	position = { 0.0f,0.0f };
	speed_vect = { 0.0f,0.0f };
	return true;
}
void Player::SelectAnim(fPoint speed_vect)
{
	if (isjumping == true)
		current_animation = &jumping_anim;
	else if (speed_vect.x != 0)
	{
		current_animation = &moving_anim;

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
		current_animation = &idle_anim;
}

bool Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();
	speed_vect = { 0, 0 };
	return true;
}

bool Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void Player::SwapLayer() {
	if (current_layer == BACK_LAYER) {
		current_layer = FRONT_LAYER;
		//scale = 1.0f;
	}
	else {
		current_layer = BACK_LAYER;
		//scale = 0.7f;
	}
}

LayerID Player::GetCurrentLayer() {
	return current_layer;
}