#include "BaseEnemy.h"
#include "j1App.h"
#include "j1Render.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Entities.h"
#include "j1Pathfinding.h"


BaseEnemy::BaseEnemy()
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
		alert_rect.x = collider_rect.x - 200;
		alert_rect.y = collider_rect.y - 50;
		alert_rect.w = collider_rect.w + 400;
		alert_rect.h = collider_rect.h + 50;


		if (App->collision->DoCollide(alert_rect, player_rect))
		{
			state = Entity::ALERT;
		}


		if (state == Entity::ALERT)
		{
			if (type == AIR && App->entities->player.current_layer == BACK_LAYER) {
				if (App->pathfinding->CreatePath(position.to_iPoint(), App->entities->player.position.to_iPoint(), current_layer) != -1) //Needs loop to track path
					path = *App->pathfinding->GetLastPath();
			}
			current_animation = &alert_anim;
			App->render->Blit(App->entities->texture, collider_rect.x + ((collider_rect.w - App->entities->exclamation.GetCurrentFrame().rect.w) / 2), collider_rect.y - 10, &App->entities->exclamation.GetCurrentFrame().rect.toSDL_Rect());

			if (player_rect.x < collider_rect.x)
			{
				flipped = true;
				Accelerate(-0.5f, 0);
			}
			else
				Accelerate(0.5f, 0);

			if (current_animation->Finished())
			{
				state = Entity::IDLE;
			}
		}
		else
		{
			alert_anim.Reset();
			state = Entity::IDLE;
			current_animation = &idle_anim;
		}
	}

	//Blit

	else alpha = 128;
	SDL_SetTextureAlphaMod(App->entities->texture, alpha);

	App->render->Blit(App->entities->texture, position.x, position.y, &current_animation->GetCurrentFrame().rect.toSDL_Rect(), 1.0f, 0, 0, 0, true, flipped);

	if (type == LARVA || type == AIR)
		LarvaBlockUpdate();
	else
		App->collision->Checkcollisions(current_layer, collider_rect, position, &speed_vect);

	Move();

	SDL_SetTextureAlphaMod(App->entities->texture, 255);

	//Gravity
	if (gravity == true)
		Accelerate(0, 0.5f);

	return true;
}

void BaseEnemy::LarvaBlockUpdate()
{
	iRect cube = App->entities->larva_cube.GetCurrentFrame().rect;
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
	App->render->Blit(App->entities->texture, position.x, position.y, &App->entities->larva_cube.GetCurrentFrame().rect.toSDL_Rect(), 1.0f, 0, 0, 0, true);
	SDL_SetTextureAlphaMod(App->entities->texture, 255);

}