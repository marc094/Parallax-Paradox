#include "j1Player.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Map.h"

j1Player::j1Player()
{
}


j1Player::~j1Player()
{
}

void j1Player::Init()
{
	active = true;
}

// Called before render is available
bool j1Player::Awake(pugi::xml_node&)
{
	bool ret = true;
	LOG("Loading Player Module");
	position = App->map->GetInitialPlayerPos();
	pugi::xml_document player_anims;
	pugi::xml_parse_result result = player_anims.load_file("animations.xml");
	pugi::xml_node doc_node;

	for (pugi::xml_node animation : doc_node.children()) {
		
		Animation aux;
		aux.name = animation.name();

		for (pugi::xml_node frames : animation.children())
		{
			SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("h").as_int(), frames.attribute("h").as_int() };
			aux.PushBack(aux_rect);
			animation_list.add(aux);
		}
	}
	if (result == NULL) {
		LOG("Could not load map xml file %s. pugi error: %s", "animations.xml", result.description());
		ret = false;
	}
	return true;
}

// Called before the first frame
bool j1Player::Start()
{

	return true;
}

// Called each loop iteration
bool j1Player::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Player::Update(float dt)
{
	return true;
}

// Called each loop iteration
bool j1Player::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1Player::CleanUp()
{
	return true;
}

bool j1Player::Load(pugi::xml_node&)
{
	return true;
}

bool j1Player::Save(pugi::xml_node&) const
{
	return true;
}