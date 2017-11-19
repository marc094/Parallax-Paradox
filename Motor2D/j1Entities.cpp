#include "j1Entities.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "Brofiler\Brofiler.h"


j1Entities::j1Entities() : j1Module()
{
	name.create("entities");
}


j1Entities::~j1Entities()
{
}

bool j1Entities::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	LOG("Loading Module Entities");

	pugi::xml_parse_result result = animations.load_file(conf.child("animations").child_value());

	if (result == NULL) {
		LOG("Could not load map xml file %s. pugi error: %s", conf.child("animations").child_value(), result.description());
		ret = false;
	}

	if (ret) {
		pugi::xml_node doc_node = animations.child("animations");
		ground_enemy_node = doc_node.child("Enemies").child("ground");
		boxer_enemy_node = doc_node.child("Enemies").child("boxer");
		larva_enemy_node = doc_node.child("Enemies").child("larva");
		air_enemy_node = doc_node.child("Enemies").child("air");
		player_node = doc_node.child("player");

		exclamation.PushBack({ 0,36,3,8 });
		larva_cube.PushBack({ 0,81,48,48});
		air_cube.PushBack({ 100,81,69,34 });
	}


	player.Awake();

	return ret;
}

bool j1Entities::Start()
{	
	texture = App->tex->Load(animations.first_child().child("spritesheet").attribute("path").as_string());

	p2List_item<BaseEnemy*>* current_enemy = Enemies.start;
	while (current_enemy != NULL)
	{
		current_enemy->data->Start();
		current_enemy = current_enemy->next;
	}

	player.Start();
	return true;
}

bool j1Entities::Update(float dt)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::IndianRed);
	p2List_item<BaseEnemy*>* current_enemy = Enemies.start;
	while (current_enemy != NULL)
	{
		current_enemy->data->Update(dt);
		current_enemy = current_enemy->next;
	}

	player.Update(dt);
	return true;
}
bool j1Entities::CleanUp()
{
	player.CleanUp();
	App->tex->UnLoad(texture);
	texture = nullptr;
	p2List_item<BaseEnemy*>* current_enemy = Enemies.start;
	while (current_enemy != NULL)
	{
		RELEASE(current_enemy->data);
		current_enemy = current_enemy->next;
	}
	Enemies.clear();

	return true;
}
bool j1Entities::Load(pugi::xml_node& data)
{
	player.Load(data);
	return true;
}

bool j1Entities::Save(pugi::xml_node& data) const
{
	player.Save(data);

	return true;
}


BaseEnemy* j1Entities::Add_Enemy(BaseEnemy::Type type, fPoint position, LayerID layer)
{
	BaseEnemy* aux = new BaseEnemy();
	aux->position = position;
	aux->state = Entity::IDLE;
	aux->speed_vect = fPoint( 0.0f, 0.0f );
	aux->max_speed = fPoint( ENEMY_MAX_SPEED_X, ENEMY_MAX_SPEED_Y);
	aux->current_layer = layer;
	aux->type = type;

	pugi::xml_node current_node;

	if (type == BaseEnemy::GROUND)
	{
		current_node = ground_enemy_node;
		aux->max_speed = fPoint(1.0f, 1.0f);
	}
	else if (type == BaseEnemy::BOXER)
	{
		current_node = boxer_enemy_node;
	}
	else if (type == BaseEnemy::LARVA)
	{
		current_node = larva_enemy_node;
	}
	else if (type == BaseEnemy::AIR)
	{
		current_node = air_enemy_node;
		aux->max_speed = fPoint(1.0f, 0.5f);
	}

	
	for (pugi::xml_node animation : current_node.child("animationInfo").children()) {

		Animation aux_anim;

		aux_anim.name = animation.name();
		aux_anim.speed = animation.attribute("duration").as_float();
		aux_anim.loop = animation.attribute("loop").as_bool();

		for (pugi::xml_node frames : animation.children())
		{
			SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("w").as_int(), frames.attribute("h").as_int() };
			aux_anim.PushBack(aux_rect);
		}

		if (!strcmp(aux_anim.name, "idle"))
			aux->idle_anim = aux_anim;
		else if (!strcmp(aux_anim.name, "walking"))
			aux->moving_anim = aux_anim;
		else if (!strcmp(aux_anim.name, "alert"))
			aux->alert_anim = aux_anim;
	}

	

	aux->gravity = current_node.child("enemyInfo").attribute("gravity").as_bool();
	aux->current_animation = &aux->idle_anim;
	aux->collider = aux->current_animation->GetCurrentFrame(0.0f).rect;

	Enemies.add(aux);
	return aux;
}