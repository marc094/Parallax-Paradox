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

bool BaseEnemy::Update(float dt)
{
	iRect collider_rect = current_animation->GetCurrentFrame().rect;
	collider_rect.x = position.x;
	collider_rect.y = position.y;

	bool flipped = false;
	Uint8 alpha = 255;

	if (App->entities->player.current_layer == current_layer)
	{
		//Check Collisions

		iRect player_rect = App->entities->player.collider;
		player_rect.x = App->entities->player.GetPosition().x;
		player_rect.y = App->entities->player.GetPosition().y;


		if (App->collision->DoCollide(collider_rect, player_rect))
			App->Reload();

		//Move

		
		iRect alert_rect;
		if (type == LARVA)
		{
			alert_rect.x = collider_rect.x - 200;
			alert_rect.y = collider_rect.y - 100;
			alert_rect.w = collider_rect.w + 400;
			alert_rect.h = collider_rect.h + 200;
		}
		else if (type == AIR)
		{
			alert_rect.x = collider_rect.x - 300;
			alert_rect.y = collider_rect.y - 200;
			alert_rect.w = collider_rect.w + 600;
			alert_rect.h = collider_rect.h + 300;
		}


		if (App->collision->DoCollide(alert_rect, player_rect))
		{
			state = Entity::ALERT;
		}


		if (state == Entity::ALERT)
		{
			accumulated_updates++;
			if (type == AIR && App->entities->player.current_layer == BACK_LAYER/* && (accumulated_updates * update_to_frame_ratio >= 1.0f || current_path_index >= path.Count())*/)
			{
				accumulated_updates = 0;
				GetPath();
			}
			else
			{
				if (player_rect.x < collider_rect.x)
				{
					flipped = true;
					Accelerate(-0.5f, 0);
				}
				else
					Accelerate(0.5f, 0);
			}

			if  (type == AIR)
				FollowPath();

			current_animation = &alert_anim;
			App->render->Blit(App->entities->texture, collider_rect.x + ((collider_rect.w - App->entities->exclamation.GetCurrentFrame().rect.w) / 2), collider_rect.y - 10, &App->entities->exclamation.GetCurrentFrame().rect.toSDL_Rect());

			
	

			if (current_animation->Finished())
			{
				state = Entity::IDLE;
			}
		}
		else
		{
			speed_vect = { 0.0f, 0.0f };
			alert_anim.Reset();
			state = Entity::IDLE;
			current_animation = &idle_anim;
		}
	}
	else alpha = 128;

	if (type != AIR)
		Move();

	//Gravity
	if (gravity == true)
		Accelerate(0, 0.5f);
	else speed_vect.y = 0.0f;

	//Blit
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);

	App->render->Blit(App->entities->texture, position.x, position.y, &current_animation->GetCurrentFrame().rect.toSDL_Rect(), 1.0f, 0, 0, 0, true, flipped);

	if (type == LARVA || type == AIR)
		LarvaBlockUpdate();
	else
		App->collision->Checkcollisions(current_layer, collider_rect, position, &speed_vect);

	SDL_SetTextureAlphaMod(App->entities->texture, 255);

	return true;
}

void BaseEnemy::LarvaBlockUpdate()
{
	Animation* current_block = &App->entities->larva_cube;
	if (type == AIR)
	{
		current_block = &App->entities->air_cube;
	}
	

	iRect cube = current_block->GetCurrentFrame().rect;
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

	App->collision->Checkcollisions(FRONT_LAYER, cube, position, &speed_vect);	
	if (App->entities->player.current_layer == FRONT_LAYER)
	{
		alpha = 255;
		App->collision->SetSpVecToCollisions(cube, player_rect, App->entities->player.speed_vect);
	}
	
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);
	App->render->Blit(App->entities->texture, cube.x, cube.y, &current_block->GetCurrentFrame().rect.toSDL_Rect(), 1.0f, 0, 0, 0, true);
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
		Accelerate(1, 1);

		Interpolate(position.x, (float)path_point.x, fabs(speed_vect.x));
		Interpolate(position.y, (float)path_point.y, fabs(speed_vect.y));

		if (path_point == position.to_iPoint() && current_path_index < path.Count())
			current_path_index++;
	}
}