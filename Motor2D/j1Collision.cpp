#include "j1Collision.h"
#include "SDL\include\SDL.h"


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

}

bool j1Collision::Start() {

}

bool j1Collision::PreUpdate() {

}

bool j1Collision::Update(float dt) {

}

bool j1Collision::PostUpdate() {

}

bool j1Collision::CleanUp() {

}

bool j1Collision::Load(pugi::xml_node&) {

}

bool j1Collision::Save(pugi::xml_node&) const {

}

void j1Collision::SetSpVecToCollisions(const SDL_Rect collider1, const  SDL_Rect collider2, fPoint &speed_vector)
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