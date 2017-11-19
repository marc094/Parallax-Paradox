#include "j1Collision.h"
#include "SDL\include\SDL.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Entities.h"

j1Collision::j1Collision()
{
	name.create("collision");
}


j1Collision::~j1Collision()
{
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
	if (App->debug) {
		BlitDebugColliders();
	}
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

bool j1Collision::Checkcollisions(const LayerID collidertype, const iRect rect_frame, const fPoint position, fPoint& speed_vector, float delta_time) const
{
	bool grounded = false;
	p2List_item<MapLayer*>* map_layer = nullptr;
	for (map_layer = App->map->data.layers.start; map_layer != NULL; map_layer = map_layer->next)
	{
		if (map_layer->data->layer == collidertype)
			break;
	}

	for (p2List_item<Collider*>* collider = map_layer->data->layer_colliders.start; collider != NULL; collider = collider->next)
	{
		iRect player_rect = rect_frame;
		player_rect.x = (int)position.x * scale;
		player_rect.y = (int)position.y * scale;
		player_rect.w *= scale;
		player_rect.h *= scale;

		iRect aux = collider->data->rect * scale;
		aux.w *= scale;
		aux.h *= scale;

		aux.x = (int)(App->render->camera.x * map_layer->data->parallax_speed * scale) + (aux.x * scale);
		aux.y = (int)(App->render->camera.y * map_layer->data->parallax_speed * scale) + (aux.y * scale);

		player_rect.x = (int)(App->render->camera.x * scale) + (player_rect.x * scale);
		player_rect.y = (int)(App->render->camera.y * scale) + (player_rect.y * scale);

		iPoint p1, p2;
		p1 = iPoint(player_rect.x + player_rect.w / 2, player_rect.y + player_rect.h / 2);
		p2 = iPoint(aux.x + aux.w / 2, aux.y + aux.h / 2);
		if (p1.DistanceNoSqrt(p2) > MAX_DISTANCE_COLLIDER_CULLING)
			continue;

		SetSpVecToCollisions(aux, player_rect, speed_vector, grounded, delta_time);
	}

	return grounded;
}

void j1Collision::BlitDebugColliders() const
{
	for (p2List_item<MapLayer*>* map_layer = App->map->data.layers.start; map_layer != NULL; map_layer = map_layer->next)
	{
		if (map_layer->data->layer != App->entities->player.current_layer)
			continue;

		for (p2List_item<Collider*>* collider = map_layer->data->layer_colliders.start; collider != NULL; collider = collider->next)
		{
			iRect aux(collider->data->rect);
			App->render->DrawQuad(aux.toSDL_Rect(), 0, 255, 0, map_layer->data->parallax_speed, 128);
		}
	}

	for (p2List_item <BaseEnemy*>* enemy = App->entities->Enemies.start; enemy != nullptr; enemy = enemy->next)
	{
		if (enemy->data->current_layer != App->entities->player.current_layer)
			continue;

		if (enemy->data->type == BaseEnemy::Type::LARVA)
		{
			iRect aux((int)enemy->data->position.x, (int)enemy->data->position.y, enemy->data->collider.w, enemy->data->collider.h);
			App->render->DrawQuad(aux.toSDL_Rect(), 255, 0, 0, App->map->data.layers[enemy->data->current_layer + 1]->parallax_speed, 128);

			iRect aux_cube = App->entities->larva_cube.frames[App->entities->larva_cube.getFrameIndex()].rect;
			aux_cube = iRect((int)enemy->data->position.x, (int)enemy->data->position.y, aux_cube.w, aux_cube.h);
			App->render->DrawQuad(aux_cube.toSDL_Rect(), 0, 255, 0, App->map->data.layers[enemy->data->current_layer + 1]->parallax_speed, 128);
		}
		else {
			iRect aux((int)enemy->data->position.x, (int)enemy->data->position.y, enemy->data->collider.w, enemy->data->collider.h);
			App->render->DrawQuad(aux.toSDL_Rect(), 255, 0, 0, 1/*App->map->data.layers[enemy->data->current_layer]->parallax_speed*/, 128);
		}
	}

	iRect aux((int)App->entities->player.position.x, (int)App->entities->player.position.y, App->entities->player.collider.w, App->entities->player.collider.h);
	App->render->DrawQuad(aux.toSDL_Rect(), 0, 0, 255, 1.0f, 128);

}

void j1Collision::SetSpVecToCollisions(const iRect collider1, const iRect collider2, fPoint &speed_vector, bool& grounded, float delta_time) const
{
	fPoint delta_applied_spd_vec = speed_vector * delta_time;
	if (collider2.x + collider2.w + delta_applied_spd_vec.x > collider1.x && collider2.x + delta_applied_spd_vec.x < collider1.x + collider1.w
		&& collider2.y + collider2.h + delta_applied_spd_vec.y > collider1.y && collider2.y + delta_applied_spd_vec.y < collider1.y + collider1.h) //there's contact
	{
		if (collider2.x < collider1.x + collider1.w && collider2.x + collider2.w > collider1.x) //collider2 is in x-axis collision with collider1
		{
			if (collider2.y + delta_applied_spd_vec.y < collider1.y + collider1.h && collider2.y + delta_applied_spd_vec.y > collider1.y && delta_applied_spd_vec.y < 0)
			{
				speed_vector.y = 0;
			}
			else if (collider2.y + collider2.h + delta_applied_spd_vec.y > collider1.y && delta_applied_spd_vec.y >= 0)
			{
				speed_vector.y = 0;
				grounded = true;
			}
		}
		if (collider2.y < collider1.y + collider1.h && collider2.y + collider2.h > collider1.y) //collider2 is in x-axis collision with collider1
		{
			if (collider2.x + delta_applied_spd_vec.x < collider1.x + collider1.w && collider2.x + delta_applied_spd_vec.x > collider1.x && delta_applied_spd_vec.x < 0)
			{
				speed_vector.x = 0;
			}
			else if (collider2.x + collider2.w + delta_applied_spd_vec.x > collider1.x && delta_applied_spd_vec.x >= 0)
			{
				speed_vector.x = 0;
			}
		}
	}
}

bool j1Collision::DoCollide(iRect collider1, iRect collider2) const
{
	SDL_Rect result;
	if ((bool)SDL_IntersectRect(&collider1.toSDL_Rect(), &collider2.toSDL_Rect(), &result))
		return true;
	return false;
}