#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Entities.h"
#include "Entity.h"
#include "Player.h"
#include "j1Textures.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	level = 1;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& data)
{
	LOG("Loading Scene");
	bool ret = true;
	max_level = 0;
	for (pugi::xml_node level : data.children())
	{
		max_level++;
		xml_file_name.PushBack(level.attribute("name").as_string());
	}
	


	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{

	

	if (level > max_level)
		level = 1;
	App->map->Load(xml_file_name[level-1].GetString());


	//if (!playing)
	//{
	//	if (level == 1)
	//		App->audio->PlayMusic("audio/music/MotherEarthBound_Zero_Remix_8_Melodies.ogg", -1);
	//	else if (level == 2)
	//		App->audio->PlayMusic("audio/music/Onett_Theme_Remastered_EarthBound.ogg", -1);

	//	playing = true;
	//}


	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		//App->player->Accelerate(0, -1);
		//App->render->camera.y -= 1;
		App->entities->player.SwapLayer();
	
	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		//App->render->camera.y += 1;
		App->entities->player.Accelerate(0, ACCELERATION, dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		//App->render->camera.x -= 1;
		App->entities->player.Accelerate(-ACCELERATION, 0, dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		//App->render->camera.x += 1;
		App->entities->player.Accelerate(ACCELERATION, 0, dt);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !App->entities->player.isJumping() && !App->entities->player.god_mode)
	{
		App->entities->player.setJumping(true);
		App->entities->player.Accelerate(0, -JUMP_FORCE, dt);
	}
	else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && App->entities->player.god_mode)
	{
		App->entities->player.Accelerate(0, -ACCELERATION, dt);
	}


	// Settings input
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		App->entities->player.god_mode = !App->entities->player.god_mode;

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (App->GetFramerateCap() == 60)
			App->SetFramerateCap(30);
		else App->SetFramerateCap(60);
	}

	//App->render->Blit(img, 0, 0);
	App->map->Draw();

	// "Map:%dx%d Tiles:%dx%d Tilesets:%d"
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	//App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		ChangeScene(1);

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		ChangeScene(level);

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		App->debug = !App->debug;



	CheckEnd();

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	xml_file_name.Clear();
	
	return true;
}

void j1Scene::ChangeScene(uint _level) {
	level = _level;
	playing = false;
	App->Reload();
}

void j1Scene::CheckEnd() {
	if (App->entities->player.GetPosition().DistanceTo(App->map->GetFinalPlayerPos()) < 30)
		App->scene->ChangeScene(level + 1);
}