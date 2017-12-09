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
#include "j1Gui.h"
#include "Button.h"
#include "Label.h"

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

	if (state == IN_GAME)
	{
		App->map->Load(xml_file_name[level - 1].GetString());
		App->entities->active = true;
	}
	else
	{
		menu_background = App->tex->Load("textures/menu background2.png");
		title = App->tex->Load("textures/title2.png");
		buttons = App->tex->Load("gui/Buttons2.png");
		uint w;
		uint h;
		App->win->GetWindowSize(w, h);

		SDL_Rect button_idle = { 1000,1000,922/3,213/3 };
		SDL_Rect button_hover = { 0,0,922/3,213/3 };
		SDL_Rect button_press = { 0,0,0,0 };

		App->gui->AddSprite( w/2, h/2, menu_background);
		App->gui->AddSprite(w / 2, (h / 2 - 100), title);
		Button* start_button = App->gui->AddButton(w / 2, 71 + (h / 2), buttons, true, &button_idle, &Game_start, &button_hover, &button_press);
		
		Label* start = App->gui->AddLabel(w/2, (h/2) + 71, 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		start->setString("START");
		start_button->setLabel(start);

		Button* exit_button = App->gui->AddButton(w / 2, 150 + (h / 2), buttons, true, &button_idle, &Game_start, &button_hover, &button_press);

		Label* exit = App->gui->AddLabel(w / 2, (h / 2) + 150, 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		exit->setString("EXIT");
		exit_button->setLabel(start);


		App->entities->active = false;
	}
		

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
	if (state == IN_GAME && !App->entities->player.hit)
	{
		CheckInput(dt);
	}

	//App->render->Blit(img, 0, 0);
	if (state == IN_GAME)
	{
		App->map->Draw(dt);
	}


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

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN)
		App->SetTimeScale(0.5f);

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_UP)
		App->SetTimeScale(1.0f);

	// Settings input
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (App->GetFramerateCap() == 60)
			App->SetFramerateCap(30);
		else App->SetFramerateCap(60);
	}

	if (state == IN_GAME)
	CheckEnd();

	else if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->Reload();
		state = IN_GAME;
	}
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
void j1Scene::CheckInput(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		//App->player->Accelerate(0, -1);
		//App->render->camera.y -= 1;
		App->entities->player.SwapLayer();

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		//App->render->camera.y += 1;
		App->entities->player.Accelerate(0, ACCELERATION, dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		//App->render->camera.x -= 1;
		App->entities->player.Accelerate(-ACCELERATION, 0, dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		//App->render->camera.x += 1;
		App->entities->player.Accelerate(ACCELERATION, 0, dt);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !App->entities->player.isJumping() && !App->entities->player.god_mode)
	{
		App->entities->player.setJumping(true);
		App->entities->player.Accelerate(0, -JUMP_FORCE, dt);
		App->audio->PlayFx(App->entities->player.jump_sound);
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && App->entities->player.god_mode)
	{
		App->entities->player.Accelerate(0, -ACCELERATION, dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		App->entities->player.god_mode = !App->entities->player.god_mode;


}

void j1Scene::CheckEnd() {
	if (App->entities->player.GetPosition().DistanceTo(App->map->GetFinalPlayerPos()) < 30)
		App->scene->ChangeScene(level + 1);
}

void Game_start()
{
	App->Reload();
	App->scene->state = App->scene->IN_GAME;
}