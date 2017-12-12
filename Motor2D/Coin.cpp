#include "Coin.h"
#include "j1App.h"
#include "j1Entities.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Render.h"

Coin::Coin()
{
}


Coin::~Coin()
{
}

bool Coin::Update(float dt)
{
	iRect collider_rect = current_animation->GetCurrentFrame(dt).rect;
	collider_rect.x = (int)position.x;
	collider_rect.y = (int)position.y;


	Uint8 alpha = 255;


	//Check Collisions

	iRect player_rect = App->entities->player.collider;
	player_rect.x = (int)App->entities->player.GetPosition().x;
	player_rect.y = (int)App->entities->player.GetPosition().y;


	if (App->collision->DoCollide(collider_rect, player_rect))
	{
		App->audio->PlayFx(App->scene->hit_sound);
		App->entities->player.coins++;
		to_delete = true;
			
	}

	SDL_SetTextureAlphaMod(App->entities->texture, alpha);

	iRect frame = current_animation->GetCurrentFrame(dt).result_rect;

	App->render->Blit(App->entities->texture, (int)position.x, (int)position.y, &frame.toSDL_Rect(), 1.0f, 0, 0, 0, true);
	SDL_SetTextureAlphaMod(App->entities->texture, 255);

		return true;
}
