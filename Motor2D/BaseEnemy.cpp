#include "BaseEnemy.h"
#include "j1App.h"
#include "j1Render.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "BaseEnemy.h"
#include "j1Player.h"


BaseEnemy::BaseEnemy()
{
}


BaseEnemy::~BaseEnemy()
{
}

bool BaseEnemy::Update(float dt)
{

	//Check Collisions
	iRect collider_rect = current_animation->GetCurrentFrame().rect;
	collider_rect.x = position.x;
	collider_rect.y = position.y;
	iRect player_rect = App->entities->player.collider;
	player_rect.x = App->entities->player.GetPosition().x;
	player_rect.y = App->entities->player.GetPosition().y;


	if (App->collision->DoCollide(collider_rect, player_rect))
		App->Reload();

	//Move
	iRect alert_rect;
	alert_rect.x = collider_rect.x - 50;
	alert_rect.y = collider_rect.y - 50;
	alert_rect.w = collider_rect.w + 100;
	alert_rect.h = collider_rect.h + 100;

	bool flipped = false;

	if (App->collision->DoCollide(alert_rect, player_rect))
	{
		state = Entity::ALERT;
	}


	if (state == Entity::ALERT)
	{
		current_animation = &alert_anim;
		App->render->Blit(App->entities->texture, collider_rect.x + ((collider_rect.w - App->entities->exclamation.GetCurrentFrame().rect.w) / 2), collider_rect.y - 10, &App->entities->exclamation.GetCurrentFrame().rect.toSDL_Rect());

		if (player_rect.x < collider_rect.x)
		{
			flipped = true;
		}

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

	App->collision->Checkcollisions(currentLayer, collider_rect, position, &speed_vect);

	Move();

	//Gravity
	Accelerate(0, 0.5f);

	//Blit
	App->render->Blit(App->entities->texture, position.x, position.y, &current_animation->GetCurrentFrame().rect.toSDL_Rect(), 1.0f, 0, 0, 0, true, flipped);

	return true;
}