#include "j1Collision.h"
#include "SDL\include\SDL.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Window.h"

j1Collision::j1Collision()
{
	name.create("collision");
}


j1Collision::~j1Collision()
{
}

void j1Collision::Init() {

}

bool j1Collision::Awake(pugi::xml_node&) {
	scale = App->win->GetScale();
	return true;
}

bool j1Collision::Start() {
	return true;
}

bool j1Collision::PreUpdate() {
	return true;
}

bool j1Collision::Update(float dt) {
	return true;
}

bool j1Collision::PostUpdate() {
	return true;
}

bool j1Collision::CleanUp() {
	return true;
}

bool j1Collision::Load(pugi::xml_node&) {
	return true;
}

bool j1Collision::Save(pugi::xml_node&) const {
	return true;
}

void j1Collision::Checkcollisions(ColliderType collidertype, Rect rect_frame, fPoint position, fPoint* speed_vector)
{
	uint j = 0;
	bool checked = false;
	while (checked == false && j < App->map->data.layers.count() && App->map->data.layers[j] != NULL)
	{
		if (App->map->data.layers[j]->layer_colliders[0]->collidertype == collidertype)
		{
			uint i = 0;
			while (i < App->map->data.layers[j]->layer_colliders.count() && App->map->data.layers[j]->layer_colliders[i] != NULL)
			{
				Rect aux = App->map->data.layers[j]->layer_colliders[i]->rect;

				fPoint collision_pos = position * App->map->data.layers[j]->parallax_speed;

				Rect player_rect = rect_frame;
				player_rect.x = position.x;
				player_rect.y = position.y;
				player_rect.w *= scale;
				player_rect.h *= scale;

				aux.w *= scale;
				aux.h *= scale;

				if (!App->debug) {
					aux.x = (int)(App->render->camera.x * App->map->data.layers[j]->parallax_speed) + (aux.x /** scale*/);
					aux.y = (int)(App->render->camera.y * App->map->data.layers[j]->parallax_speed) + (aux.y /** scale*/);

					player_rect.x = (int)(App->render->camera.x * 1.0f) + (player_rect.x /** scale*/);
					player_rect.y = (int)(App->render->camera.y * 1.0f) + (player_rect.y /** scale*/);
				}
				else {
					App->render->DrawQuad(aux.toSDL(), 0, 255, 0, App->map->data.layers[j]->parallax_speed, 128);
					App->render->DrawQuad(player_rect.toSDL(), 255, 0, 0, 1.0f, 128);
					
					aux.x = (int)(App->render->camera.x * App->map->data.layers[j]->parallax_speed) + (aux.x /** scale*/);
					aux.y = (int)(App->render->camera.y * App->map->data.layers[j]->parallax_speed) + (aux.y /** scale*/);

					player_rect.x = (int)(App->render->camera.x * 1.0f) + (player_rect.x /** scale*/);
					player_rect.y = (int)(App->render->camera.y * 1.0f) + (player_rect.y /** scale*/);
				}

				SetSpVecToCollisions(aux, player_rect, *speed_vector);
				
				i++;
				checked = true;
			}
		}
		j++;
	}

}
void j1Collision::SetSpVecToCollisions(const Rect collider1, const  Rect collider2, fPoint &speed_vector)
{
	if (collider2.x + collider2.w + speed_vector.x > collider1.x && collider2.x + speed_vector.x < collider1.x + collider1.w
		&& collider2.y + collider2.h + speed_vector.y > collider1.y && collider2.y + speed_vector.y < collider1.y + collider1.h) //there's contact
	{
		if (collider2.x < collider1.x + collider1.w && collider2.x + collider2.w > collider1.x) //collider2 is in x-axis collision with collider1
		{
			if (collider2.y + speed_vector.y < collider1.y + collider1.h && collider2.y + speed_vector.y > collider1.y && speed_vector.y < 0)
			{
				speed_vector.y = 0;
			}
			else if (collider2.y + collider2.h + speed_vector.y > collider1.y && speed_vector.y >= 0)
			{
				speed_vector.y = 0;
			}
		}
		if (collider2.y < collider1.y + collider2.h && collider2.y + collider2.h > collider1.y)
		{
			if (collider2.x + speed_vector.x < collider1.x + collider1.w && collider2.x + collider2.w + speed_vector.x > collider1.x + collider1.w && speed_vector.x < 0)
			{
				speed_vector.x = 0;
			}
			else if (collider2.x + collider2.w + speed_vector.x > collider1.x && speed_vector.x >= 0)
			{
				speed_vector.x = 0;
			}
		}
	}

}
bool j1Collision::CheckEnemyCollisions(const Rect collider1, const Rect collider2)
{
	bool ret = false;
	if (collider2.x + collider2.w   > collider1.x && collider2.x  < collider1.x + collider1.w
		&& collider2.y + collider2.h > collider1.y && collider2.y < collider1.y + collider1.h) //there's contact
	{
		if (collider2.x < collider1.x + collider1.w && collider2.x + collider2.w > collider1.x) //collider2 is in x-axis collision with collider1
		{
			if (collider2.y < collider1.y + collider1.h && collider2.y > collider1.y)
			{
				ret = true;
			}
			else if (collider2.y + collider2.h  > collider1.y )
			{
				ret = true;
			}
		}
		if (collider2.y < collider1.y + collider2.h && collider2.y + collider2.h > collider1.y)
		{
			if (collider2.x < collider1.x + collider1.w && collider2.x + collider2.w > collider1.x + collider1.w)
			{
				ret = true;
			}
			else if (collider2.x + collider2.w > collider1.x)
			{
				ret = true;
			}
		}
	}
	return ret;
}