#include "Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Entities.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Window.h"

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

	speed_vect = { 0.0f, 0.0f };

	god_mode_aura.PushBack({ 79, 45, 30, 30 });

	for (pugi::xml_node animation : doc_node.children()) {

		Animation aux/* = new Animation()*/;

		aux.name = animation.name();
		aux.speed = animation.attribute("duration").as_float();
		aux.loop = animation.attribute("loop").as_bool();

		for (pugi::xml_node frames : animation.children())
		{
			SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("w").as_int(), frames.attribute("h").as_int() };
			iPoint pivot(frames.attribute("pivot_x").as_int(), frames.attribute("pivot_y").as_int());
			aux.PushBack(aux_rect, pivot);
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
	collider.entity = this;
	collider.rect = current_animation->GetCurrentFrame(0.0f).rect;
	max_speed = fPoint(MAX_SPEED_X, MAX_SPEED_Y);
	god_mode = false;
	is_jumping = false;
	grounded = false;
	lives = 3;
	coins = 0;

	return true;
}

bool Player::Update(float dt)
{
	SelectAnim(speed_vect);

	current_animation->GetCurrentFrame(dt);
	god_mode_aura.GetCurrentFrame(dt);
	collider.rect.x = (int)(position.x) - (int)(App->render->camera.x * camera_speed);
	collider.rect.y = (int)(position.y) - (int)(App->render->camera.y * camera_speed);
	iPoint collision_side = App->collision->Checkcollisions(collider, dt);
	grounded = grounded | (collision_side.y == 1);

	if (abs(collision_side.x != 0)) speed_vect.x = 0;
	if (abs(collision_side.y != 0)) speed_vect.y = 0;


	if (onhit_timer.Count(0.2f))
	{
		hit = false;
		max_speed.x = MAX_SPEED_X;
		max_speed.y = MAX_SPEED_Y;
		color = { 255,0,0 };

	}

	if (grounded && is_jumping)
	{
		is_jumping = false;
		landing_anim.Reset();
		current_animation = &landing_anim;
	}

	Move(dt);
	Break(dt);

	//Gravity
	Accelerate(0, GRAVITY, dt);

	aura_angle += 90.0f * dt;

	float camera_speed = 310.0f * dt;
	fPoint cam_vec, c;
	float scale = App->win->GetScale();
	c.x = (position.x * scale) - ((App->render->camera.w) / 2.0f);
	c.y = (position.y * scale) - ((App->render->camera.h) / 2.0f);
	cam_vec.x = c.x - App->render->camera.x;
	cam_vec.y = c.y - App->render->camera.y;
	float angle = cam_vec.angle();

	App->render->camera.x = Interpolate(App->render->camera.x, c.x, abs(camera_speed * cos(angle)));
	App->render->camera.y = Interpolate(App->render->camera.y, c.y, abs(camera_speed * sin(angle)));

	if (position.y > 1400)
	{
		App->Reload();
		App->audio->PlayFx(App->scene->hit_sound);
		coins = 0;
	}

	if (lives <= 0)
	{
		coins = 0;
		App->Reload();
	}
		

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

	position = { 0.0f, 0.0f };
	speed_vect = { 0.0f, 0.0f };
	return true;
}

void Player::SelectAnim(fPoint speed_vect)
{
	if (is_jumping)
	{
		current_animation = &jumping_anim;

		if (speed_vect.x > 0)
		{
			flipped = false;
		}
		else if (speed_vect.x < 0)
		{
			flipped = true;
		}

	}
	else if (current_animation == &landing_anim)
	{
		if (landing_anim.Finished())
		{
			current_animation = &idle_anim;
		}
	}
	else if (speed_vect.x != 0)
	{
		if (current_animation != &landing_anim)
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
	{		
		current_animation = &idle_anim;
	}
		
}

bool Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_float();
	position.y = data.child("position").attribute("y").as_float();
	coins = data.child("coins").attribute("amount").as_int();
	App->scene->SumCoin();
	speed_vect = { 0, 0 };
	return true;
}

bool Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	pugi::xml_node coin = data.append_child("coins");
	coin.append_attribute("amount") = coins;

	return true;
}

void Player::SwapLayer() {
	App->audio->PlayFx(App->scene->change_sound);
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

void Player::OnHit(iRect collider,fPoint collider_spv, float dt)
{
	hit = true;
	invulnerable = true;
	onhit_timer.Start();
	fPoint sp = speed_vect - collider_spv;
	iRect player = current_animation->CurrentFrame().rect;
	player.x = (int)position.x;
	player.y = (int)position.y;
	max_speed.x = 500;
	max_speed.y = 500;
	App->collision->GetCollisionSide(collider, player, speed_vect, dt);
	Accelerate((-10 * sp.x), (-10 * sp.y), dt);
	
	SDL_SetTextureColorMod(App->entities->texture, 255, 0, 0);
	//speed_vect = sp * 30;
}

void Player::BlitPlayer() 
{
	App->render->Blit(App->entities->texture, (int)position.x, (int)position.y, &current_animation->CurrentFrame().rect.toSDL_Rect(), 1.0f, 0, current_animation->CurrentFrame().pivot.x, current_animation->CurrentFrame().pivot.y, flipped, true, color);
	
	if (god_mode)
	{
		iRect frame_rect = god_mode_aura.CurrentFrame().rect;
		App->render->Blit(App->entities->texture, (int)position.x + (collider.rect.w / 2) - (frame_rect.w / 2), (int)position.y + (collider.rect.h / 2) - (frame_rect.h / 2), &frame_rect.toSDL_Rect(), 1.0f, aura_angle);
	}

}