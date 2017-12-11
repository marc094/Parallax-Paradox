#include <math.h>
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
#include "Window.h"

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
		uint w;
		uint h;
		App->win->GetWindowSize(w, h);

		App->map->Load(xml_file_name[level - 1].GetString());
		lifes_sprite = App->tex->Load("textures/Lifeicon.png");
		Sprite* first_life = App->gui->AddSprite(50, h - 50, lifes_sprite, true);

		Sprite* second_life = App->gui->AddSprite(110, h - 50, lifes_sprite, true);

		Sprite* third_life = App->gui->AddSprite(170, h - 50, lifes_sprite, true);

		lives.add(first_life);
		lives.add(second_life);
		lives.add(third_life);

		time_lab = App->gui->AddLabel(w - 50, h  - 50, 40, "gui/Earth 2073.ttf", { 255,255,255,255 });
		time_lab->setAlignment(Label::LEFT);
		time.Start();

		App->entities->active = true;
	}
	else
	{
		menu_background = App->tex->Load("textures/menu background2.png");
		title = App->tex->Load("textures/title2.png");
		buttons = App->tex->Load("gui/Buttons2.png");
		credits_win = App->tex->Load("gui/Window2.png");
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

		menu_buttons.add(start_button);

		Button* continue_button = App->gui->AddButton(w / 2, 142+ (h / 2), buttons, true, &button_idle, &Game_continue, &button_hover, &button_press);

		Label* continue_label = App->gui->AddLabel(w / 2, (h / 2) + 142 , 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		continue_label->setString("CONTINUE");
		continue_button->setLabel(continue_label);

		menu_buttons.add(continue_button);

		Button* credits_button = App->gui->AddButton(w / 2, 142 + 71 + (h / 2), buttons, true, &button_idle, &Show_Credits, &button_hover, &button_press);

		Label* credit = App->gui->AddLabel(w / 2, (h / 2) + 142 + 71, 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		credit->setString("CREDITS");
		credits_button->setLabel(credit);
		App->entities->active = false;

		menu_buttons.add(credits_button);

		Button* exit_button = App->gui->AddButton(w / 2, 142 + 71 +71 + (h / 2), buttons, true, &button_idle, &Game_exit, &button_hover, &button_press);

		Label* exit = App->gui->AddLabel(w / 2, (h / 2) + 142 +71+ 71, 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		exit->setString("EXIT");
		exit_button->setLabel(exit);
		App->entities->active = false;

		menu_buttons.add(exit_button);
		credits_window = App->gui->AddWindow(w / 2, h / 2, credits_win, false);
	}
		

	//if (!playing)
	//{
	//	if (level == 1)
	//		App->audio->PlayMusic("audio/music/MotherEarthBound_Zero_Remix_8_Melodies.ogg", -1);
	//	else if (level == 2)
	//		App->audio->PlayMusic("audio/music/Onett_Theme_Remastered_EarthBound.ogg", -1);

	//	playing = true;
	//}

	/*SDL_Rect win_rect{ 0, 512, 484, 512 };
	SDL_Texture* tex = App->tex->Load("gui/atlas.png");
	Window* win = App->gui->AddWindow(0.5f * App->gui->GetGuiSize().x, 0.5f * App->gui->GetGuiSize().y, tex, true, &win_rect);

	SDL_Rect idle{ 0, 110, 230, 71 };
	SDL_Rect hovered{ 411, 166, 230, 71 };
	SDL_Rect pressed{ 641, 166, 230, 71 };
	Button* butt = App->gui->AddButton(0.5f * win->rect.w, 0.5f * win->rect.h, tex, true, &idle, &button_callback, &hovered, &pressed);
	butt->SetParent(win);
	butt->SetAnchor(1.0f, 1.0f);
	Label* text = App->gui->AddLabel(0.5 * butt->rect.w, 0.5f * butt->rect.h,
		30, "fonts/open_sans/OpenSans-Bold.ttf", { 200, 200, 200, 255 }, Label::RenderMode::BLENDED, "Button #%d", 1);
	text->SetParent(butt);*/

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
		time_lab->setString("%.2f", time.ReadSec());

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

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (!credits_bool)
			ret = false;

		else
			Hide_Credits();

	}


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


bool j1Scene::Load(pugi::xml_node& data) 
{
	level = data.child("level").attribute("current_level").as_int();
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("level");

	pos.append_attribute("current_level") = level;

	return true;
}

void j1Scene::ChangeLifes()
{
	p2List_item<Sprite*>* curr = lives.end;
	while (curr->prev != NULL)
	{
		if (curr->data->enabled)
		{
			curr->data->Enable(false);
			break;
		}
		curr = curr->prev;
	}

}

void button_callback(const char* text) {
	LOG("%s", text);
}

void Game_start()
{
	App->Reload();
	App->scene->state = App->scene->IN_GAME;
}

void Game_exit()
{
	App->scene->ret = false;
}

void Game_continue()
{
	App->LoadGame();
	App->Reload();
	App->scene->state = App->scene->IN_GAME;
}

void Show_Credits()
{
	App->scene->credits_window->Enable(true);
	App->scene->credits_bool = true;
}

void Hide_Credits()
{
	App->scene->credits_window->Enable(false);
	App->scene->credits_bool = false;
}