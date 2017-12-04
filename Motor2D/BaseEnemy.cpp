#include "BaseEnemy.h"
#include "j1App.h"
#include "j1Render.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Entities.h"
#include "j1Pathfinding.h"
#include "j1Audio.h"


BaseEnemy::BaseEnemy() : path(0)
{
}


BaseEnemy::~BaseEnemy()
{
}
bool BaseEnemy::Start()
{
	collider = current_animation->GetCurrentFrame(0.0f).rect;
	return true;
}
bool BaseEnemy::Update(float dt)
{
	iRect collider_rect = current_animation->GetCurrentFrame(dt).rect;
	collider_rect.x = (int)position.x;
	collider_rect.y = (int)position.y;

	collider.x = (int)position.x;
	collider.y = (int)position.y;

	bool flipped = false;
	Uint8 alpha = 255;

	if (App->entities->player.current_layer == current_layer)
	{
		//Check Collisions

		iRect player_rect = App->entities->player.collider;
		player_rect.x = (int)App->entities->player.GetPosition().x;
		player_rect.y = (int)App->entities->player.GetPosition().y;


		if (App->collision->DoCollide(collider, player_rect) && !App->entities->player.god_mode)
		{
			App->audio->PlayFx(App->entities->player.hit_sound);
			App->entities->player.OnHit(collider, speed_vect, dt);
		}


		//Move

		iRect alert_rect;
		if (type == LARVA)
		{
			alert_rect.x = collider.x - 200;
			alert_rect.y = collider.y - 100;
			alert_rect.w = collider.w + 400;
			alert_rect.h = collider.h + 200;
		}
		else if (type == AIR)
		{
			alert_rect.x = collider.x - 350;
			alert_rect.y = collider.y - 200;
			alert_rect.w = collider.w + 650;
			alert_rect.h = collider.h + 300;
		}
		else
		{
			alert_rect.x = collider.x - 100;
			alert_rect.y = collider.y - 50;
			alert_rect.w = collider.w + 200;
			alert_rect.h = collider.h + 100;
		}


		if (App->collision->DoCollide(alert_rect, player_rect))
		{
			if (state != RUNNING)
				state = Entity::ALERT;

			iRect exclamation_frame_rect = App->entities->exclamation.GetCurrentFrame(dt).rect;
			App->render->Blit(App->entities->texture, collider_rect.x + ((collider_rect.w - exclamation_frame_rect.w) / 2), collider_rect.y - 10, &exclamation_frame_rect.toSDL_Rect());

		}
		else
			state = IDLE;

		accumulated_updates++;
		if (state == Entity::ALERT)
		{
			current_animation = &alert_anim;
		
			if (current_animation->Finished())
			{
				state = Entity::RUNNING;
			}
		}
		else if (state == Entity::RUNNING)
		{
			if (type == AIR && App->entities->player.current_layer == BACK_LAYER && accumulated_updates * update_to_frame_ratio >= 1.0f /*|| current_path_index >= path.Count())*/)
			{
				accumulated_updates = 0;
				GetPath();
			}
			else if(type == GROUND && App->entities->player.current_layer == current_layer && accumulated_updates * update_to_frame_ratio >= 1.0f)
			{
				accumulated_updates = 0;
				GetPath();
				speed_vect.x = 0;
			}
			else
			{
				if (player_rect.x < collider.x)
				{
					flipped = true;
					Accelerate(-ACCELERATION, 0, dt);
				}
				else
					Accelerate(ACCELERATION, 0, dt);
			}

			if  (type == AIR || type == GROUND)
				FollowPath(dt);

			current_animation = &moving_anim;
		}
		else
		{
			//speed_vect = { 0.0f, 0.0f };
			alert_anim.Reset();
			moving_anim.Reset();
			state = Entity::IDLE;
			current_animation = &idle_anim;
		}
	}
	else alpha = 128;

	//Blit
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);

	iRect frame = current_animation->GetCurrentFrame(dt).result_rect;

	App->render->Blit(App->entities->texture, (int)position.x, (int)position.y, &frame.toSDL_Rect(), 1.0f, 0, 0, 0, true, flipped);
	SDL_SetTextureAlphaMod(App->entities->texture, 255);

	if (type == LARVA || type == AIR)
		LarvaBlockUpdate(dt);
	else
		App->collision->Checkcollisions(current_layer, collider, position, speed_vect, dt);

	if (type != AIR && type != GROUND)
		Move(dt);
	if (type == GROUND) position.y += speed_vect.y * dt;
	Break(dt);

	//Gravity
	if (gravity == true)
		Accelerate(0, GRAVITY, dt);
	else speed_vect.y = 0.0f;

	return true;
}

void BaseEnemy::LarvaBlockUpdate(float dt)
{
	Animation* current_block = &App->entities->larva_cube;
	if (type == AIR)
	{
		current_block = &App->entities->air_cube;
	}
	

	iRect cube = current_block->GetCurrentFrame(dt).rect;
	if (type == AIR)
	{
		cube.x = (int)position.x - 10;
		cube.h = 12;
	}
	else cube.x = (int)position.x;

	cube.y = (int)position.y;

	iRect player_rect = App->entities->player.collider;
	player_rect.x = (int)App->entities->player.GetPosition().x;
	player_rect.y = (int)App->entities->player.GetPosition().y;
	
	uint alpha = 64;

	App->collision->Checkcollisions(FRONT_LAYER, cube, position, speed_vect, dt);
	if (App->entities->player.current_layer == FRONT_LAYER)
	{
		alpha = 255;
		bool p_grounded = false;
		App->collision->SetSpVecToCollisions(cube, player_rect, App->entities->player.speed_vect, p_grounded, dt);

		App->entities->player.grounded = p_grounded | App->entities->player.grounded;
	}
	
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);
	App->render->Blit(App->entities->texture, cube.x, cube.y, &current_block->GetCurrentFrame(dt).rect.toSDL_Rect(), 1.0f, 0, 0, 0, true);
	SDL_SetTextureAlphaMod(App->entities->texture, 255);

}

void BaseEnemy::GetPath()
{
	current_path_index = 0;
	iPoint player_dims(App->entities->player.collider.w / 2, App->entities->player.collider.h / 2);
	App->pathfinding->CreatePath(App->map->WorldToMap(position.to_iPoint()), App->map->WorldToMap(player_dims + App->entities->player.position.to_iPoint()), FRONT_LAYER, &path, (type == GROUND));
}

void BaseEnemy::FollowPath(float delta_time)
{
	int path_length = path.Count();
	if (path_length > 0 && current_path_index < path_length) {
		iPoint path_point(App->map->MapToWorld(path[current_path_index]));

		Accelerate(ACCELERATION, ACCELERATION, delta_time);

		position.x = Interpolate(position.x, (float)path_point.x, fabs(speed_vect.x * delta_time));
		position.y = Interpolate(position.y, (float)path_point.y, fabs(speed_vect.y * delta_time));

		if (path_point == position.to_iPoint())
			current_path_index++;

		if (App->debug == true) {
			for (uint i = 0; i < path_length; i++) {
				iPoint pos = App->map->MapToWorld(path[i]);
				iRect aux_rect;
				aux_rect.x = pos.x;
				aux_rect.y = pos.y;
				aux_rect.w = aux_rect.h = 16;
				App->render->DrawQuad(aux_rect.toSDL_Rect(), 255, 255, 0, 1.0f, 128);
			}
		}
	}
}