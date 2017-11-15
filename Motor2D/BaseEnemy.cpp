#include "BaseEnemy.h"
#include "j1App.h"
#include "j1Render.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Entities.h"
#include "j1Pathfinding.h"


BaseEnemy::BaseEnemy() : path(0)
{
}


BaseEnemy::~BaseEnemy()
{
}
bool BaseEnemy::Start()
{
	enemyrect = current_animation->GetCurrentFrame(0.0f).rect;
	return true;
}
bool BaseEnemy::Update(float dt)
{
	iRect collider_rect = current_animation->GetCurrentFrame(dt).rect;
	collider_rect.x = position.x;
	collider_rect.y = position.y;

	enemyrect.x = position.x;
	enemyrect.y = position.y;

	bool flipped = false;
	Uint8 alpha = 255;

	if (App->entities->player.current_layer == current_layer)
	{
		//Check Collisions

		iRect player_rect = App->entities->player.collider;
		player_rect.x = App->entities->player.GetPosition().x;
		player_rect.y = App->entities->player.GetPosition().y;


		if (App->collision->DoCollide(enemyrect, player_rect) && !App->entities->player.god_mode)
			App->Reload();

		//Move

		iRect alert_rect;
		if (type == LARVA)
		{
			alert_rect.x = enemyrect.x - 200;
			alert_rect.y = enemyrect.y - 100;
			alert_rect.w = enemyrect.w + 400;
			alert_rect.h = enemyrect.h + 200;
		}
		else if (type == AIR)
		{
			alert_rect.x = enemyrect.x - 350;
			alert_rect.y = enemyrect.y - 200;
			alert_rect.w = enemyrect.w + 650;
			alert_rect.h = enemyrect.h + 300;
		}
		else
		{
			alert_rect.x = enemyrect.x - 100;
			alert_rect.y = enemyrect.y - 50;
			alert_rect.w = enemyrect.w + 200;
			alert_rect.h = enemyrect.h + 100;
		}


		if (App->collision->DoCollide(alert_rect, player_rect))
		{
			if (state != RUNNING)
			state = Entity::ALERT;

			iRect exclamation_frame_rect = App->entities->exclamation.GetCurrentFrame(dt).rect;
			App->render->Blit(App->entities->texture, collider_rect.x + ((collider_rect.w - exclamation_frame_rect.w) / 2), collider_rect.y - 10, &exclamation_frame_rect.toSDL_Rect());

		}


		if (state == Entity::ALERT)
		{
			//accumulated_updates++;
			current_animation = &alert_anim;
		
			if (current_animation->Finished())
			{
				state = Entity::RUNNING;
			}
		}
		else if (state == Entity::RUNNING)
		{
			if (type == AIR && App->entities->player.current_layer == BACK_LAYER/* && (accumulated_updates * update_to_frame_ratio >= 1.0f || current_path_index >= path.Count())*/)
			{
				//accumulated_updates = 0;
				GetPath();
			}
			else
			{
				if (player_rect.x < enemyrect.x)
				{
					flipped = true;
					Accelerate(-ACCELERATION, 0, dt);
				}
				else
					Accelerate(ACCELERATION, 0, dt);
			}

			if  (type == AIR)
				FollowPath();

			current_animation = &moving_anim;
		}
		else
		{
			speed_vect = { 0.0f, 0.0f };
			alert_anim.Reset();
			moving_anim.Reset();
			state = Entity::IDLE;
			current_animation = &idle_anim;
		}
	}
	else alpha = 128;

	if (type == LARVA || type == AIR)
		LarvaBlockUpdate(dt);
	else
		App->collision->Checkcollisions(current_layer, enemyrect, position, speed_vect);

	if (type != AIR)
		Move(dt);

	//Gravity
	if (gravity == true)
		Accelerate(0, GRAVITY, dt);
	else speed_vect.y = 0.0f;

	//Blit
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);
	App->render->Blit(App->entities->texture, position.x, position.y, &current_animation->GetCurrentFrame(dt).rect.toSDL_Rect(), 1.0f, 0, 0, 0, true, flipped);
	SDL_SetTextureAlphaMod(App->entities->texture, 255);

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
		cube.x = position.x - 10;
		cube.h = 12;
	}
	else
		cube.x = position.x;


	cube.y = position.y;
	


	iRect player_rect = App->entities->player.collider;
	player_rect.x = App->entities->player.GetPosition().x;
	player_rect.y = App->entities->player.GetPosition().y;
	
	uint alpha = 64;

	App->collision->Checkcollisions(FRONT_LAYER, cube, position, speed_vect);	
	if (App->entities->player.current_layer == FRONT_LAYER)
	{
		alpha = 255;
		App->collision->SetSpVecToCollisions(cube, player_rect, App->entities->player.speed_vect, grounded);
	}
	
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);
	App->render->Blit(App->entities->texture, cube.x, cube.y, &current_block->GetCurrentFrame(dt).rect.toSDL_Rect(), 1.0f, 0, 0, 0, true);
	SDL_SetTextureAlphaMod(App->entities->texture, 255);

}

void BaseEnemy::GetPath()
{
	current_path_index = 0;
	iPoint player_dims(App->entities->player.collider.w / 2, App->entities->player.collider.h / 2);
	App->pathfinding->CreatePath(App->map->WorldToMap(position.to_iPoint()), App->map->WorldToMap(player_dims) + App->map->WorldToMap(App->entities->player.position.to_iPoint()), FRONT_LAYER, &path); //Needs loop to track path
}

void BaseEnemy::FollowPath()
{
	if (path.Count() > 0) {
		iPoint path_point(App->map->MapToWorld(path[current_path_index]));

		//fPoint displacement((float)path_point.x - position.x, (float)path_point.y - position.y);

		//Accelerate((displacement.x != 0.0f) ? displacement.x / fabs(displacement.x) : 0.0f, (displacement.y != 0.0f) ? displacement.y / fabs(displacement.y) : 0.0f);
		Accelerate(ACCELERATION, ACCELERATION, 1.0f);

		Interpolate(position.x, (float)path_point.x, fabs(speed_vect.x));
		Interpolate(position.y, (float)path_point.y, fabs(speed_vect.y));

		if (path_point == position.to_iPoint() && current_path_index < path.Count())
			current_path_index++;
	}
}