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

bool j1Collision::Start() {
	scale = App->win->GetScale();
	return true;
}

bool j1Collision::PostUpdate() {
	if (App->debug) {
		BlitDebugColliders();
	}
	return true;
}

iPoint j1Collision::Checkcollisions(Collider c1, float delta_time) const
{
	iPoint side_collided;
	p2List_item<MapLayer*>* map_layer = nullptr;
	for (map_layer = App->map->data.layers.start; map_layer != NULL; map_layer = map_layer->next)
	{
		if (map_layer->data->layer == c1.layer)
			break;
	}

	iPoint p1(c1.rect.x + c1.rect.w / 2, c1.rect.y + c1.rect.h / 2);

	for (p2List_item<Collider*>* collider = map_layer->data->layer_colliders.start; collider != NULL; collider = collider->next)
	{
		iRect aux = collider->data->rect;
		aux.x -= (int)(App->render->camera.x * map_layer->data->parallax_speed);
		aux.y -= (int)(App->render->camera.y * map_layer->data->parallax_speed);
		//aux = aux * scale;

		iPoint p2(aux.x + aux.w / 2, aux.y + aux.h / 2);
		if (p1.DistanceNoSqrt(p2) > MAX_DISTANCE_COLLIDER_CULLING * scale)
			continue;

		if (!DoCollide(c1.rect, aux))
			continue;

		side_collided = GetCollisionSide(c1.rect, aux, c1.entity->speed_vect, delta_time);
	}

	return side_collided;
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
			App->render->DrawQuad(aux.toSDL_Rect(), 0, 255, 0, 128, map_layer->data->parallax_speed, true, true, true);
		}
	}

	for (p2List_item <BaseEnemy*>* enemy = App->entities->Enemies.start; enemy != nullptr; enemy = enemy->next)
	{
		if (enemy->data->current_layer != App->entities->player.current_layer)
			continue;

		if (enemy->data->type == BaseEnemy::Type::LARVA)
		{
			iRect aux((int)enemy->data->position.x, (int)enemy->data->position.y, enemy->data->collider.rect.w, enemy->data->collider.rect.h);
			App->render->DrawQuad(aux.toSDL_Rect(), 255, 0, 0, 128, App->map->data.layers[enemy->data->current_layer + 1]->parallax_speed);

			iRect aux_cube = App->entities->larva_cube.frames[App->entities->larva_cube.getFrameIndex()].rect;
			aux_cube = iRect((int)enemy->data->position.x, (int)enemy->data->position.y, aux_cube.w, aux_cube.h);
			App->render->DrawQuad(aux_cube.toSDL_Rect(), 0, 255, 0, 128, App->map->data.layers[enemy->data->current_layer + 1]->parallax_speed);
		}
		else {
			iRect aux((int)enemy->data->position.x, (int)enemy->data->position.y, enemy->data->collider.rect.w, enemy->data->collider.rect.h);
			App->render->DrawQuad(aux.toSDL_Rect(), 255, 0, 0, 128, 1/*App->map->data.layers[enemy->data->current_layer]->parallax_speed*/);
		}
	}

	iRect aux((int)App->entities->player.position.x, (int)App->entities->player.position.y, App->entities->player.collider.rect.w, App->entities->player.collider.rect.h);
	App->render->DrawQuad(aux.toSDL_Rect(), 0, 0, 255, 128, 1.0f);

}

iPoint j1Collision::GetCollisionSide(const iRect entity, const iRect collider, fPoint speed_vector, float delta_time) const
{
	iPoint collision_side = iPoint(0,0);
	fPoint delta_applied_spd_vec = speed_vector * delta_time;
	if (DoCollide(entity + delta_applied_spd_vec.to_iPoint(), collider)) //there's contact
	{
		if (entity.x < collider.x + collider.w && entity.x + entity.w > collider.x) //entity is in x-axis collision with collider
		{
			if (entity.y + delta_applied_spd_vec.y < collider.y + collider.h && entity.y + delta_applied_spd_vec.y > collider.y && delta_applied_spd_vec.y < 0)
			{
				collision_side.y = -1;
			}
			else if (entity.y + entity.h + delta_applied_spd_vec.y > collider.y && delta_applied_spd_vec.y >= 0)
			{
				collision_side.y = +1;
			}
		}
		if (entity.y < collider.y + collider.h && entity.y + entity.h > collider.y) //entity is in x-axis collision with collider
		{
			if (entity.x + delta_applied_spd_vec.x < collider.x + collider.w && entity.x + delta_applied_spd_vec.x > collider.x && delta_applied_spd_vec.x < 0)
			{
				collision_side.x = -1;
			}
			else if (entity.x + entity.w + delta_applied_spd_vec.x > collider.x && delta_applied_spd_vec.x >= 0)
			{
				collision_side.x = +1;
			}
		}
	}
	return collision_side;
}

bool j1Collision::DoCollide(iRect collider1, iRect collider2) const
{
	SDL_Rect result;
	if ((bool)SDL_IntersectRect(&collider1.toSDL_Rect(), &collider2.toSDL_Rect(), &result))
		return true;
	return false;
}