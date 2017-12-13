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
#include "Slider.h"

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
		SDL_Rect life = { 0,0,45,48 };
		SDL_Rect coin_rect = { 45,0,36,48 };
		Sprite* first_life = App->gui->AddSprite(50, h - 50, lifes_sprite, true,&life);

		Sprite* second_life = App->gui->AddSprite(110, h - 50, lifes_sprite, true, &life);

		Sprite* third_life = App->gui->AddSprite(170, h - 50, lifes_sprite, true, &life);

		lives.add(first_life);
		lives.add(second_life);
		lives.add(third_life);

		time_lab = App->gui->AddLabel(w - 150, h  - 50, 40, "gui/Earth 2073.ttf", { 255,255,255,255 },Label::BLENDED);
		time_lab->SetAnchor(0.0f, 0.5f);
		time.Start();

		Sprite* Coin = App->gui->AddSprite(270, h - 50, lifes_sprite, true, &coin_rect);

		coin_lab = App->gui->AddLabel(300, h - 50,40, "gui/Earth 2073.ttf", { 255,255,255,255 }, Label::BLENDED);
		coin_lab->SetAnchor(0.0f, 0.5f);
		coin_lab->setString("X%d", App->entities->player.coins);

		jump_sound = App->audio->LoadFx("audio/FX/Jump.wav");
		change_sound = App->audio->LoadFx("audio/FX/Change2.wav");
		hit_sound = App->audio->LoadFx("audio/FX/Onhit.wav");
		level_sound = App->audio->LoadFx("audio/FX/Wierd.wav");

		if (jump_sound == 0 || change_sound == 0 || hit_sound == 0 || level_sound == 0)
			LOG("Error loading sound fx: %s\n", Mix_GetError());

		App->audio->PlayFx(level_sound);

		App->entities->active = true;
	}
	else
	{
		App->entities->active = false;
		menu_background = App->tex->Load("textures/menu background2.png");
		title = App->tex->Load("textures/title2.png");
		buttons = App->tex->Load("gui/Buttons2.png");
		credits_win = App->tex->Load("gui/Credits_win.png");
		settings_win = App->tex->Load("gui/Settings win.png");
		credits_tex = App->tex->Load("gui/Credits 3.png");
		sliders = App->tex->Load("gui/Scroll.png");

		SDL_Rect slider_bar = { 0,0,20,435 };
		SDL_Rect slider_idle = { 33,0,16,94 };
		SDL_Rect slider_hover = { 33,103,16,94 };
		SDL_Rect slider_pressed = { 33,212,16,94 };

		uint w;
		uint h;
		App->win->GetWindowSize(w, h);

		SDL_Rect button_idle = { 1000,1000,250,58 };
		SDL_Rect button_hover = { 0,0,250,58 };
		SDL_Rect button_press = { 0,58,250,58 };

		App->gui->AddSprite( w/2, h/2, menu_background);
		Sprite* title_spr = App->gui->AddSprite(w / 2, (h / 2 - 100), title);
		Button* start_button = App->gui->AddButton((w / 2), 60 + (h / 2), buttons, true, &button_idle, &Game_start, &button_hover, &button_press);
		

		Label* start = App->gui->AddLabel(start_button->content_rect.w/2, (start_button->content_rect.h/2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		start->setString("START");
		start->SetParent(start_button);
		//start_button->setLabel(start);

		menu_buttons.add(start_button);

		Button* continue_button = App->gui->AddButton(w / 2, 120+ (h / 2), buttons, true, &button_idle, &Game_continue, &button_hover, &button_press);

		Label* continue_label = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		continue_label->setString("CONTINUE");
		continue_label->SetParent(continue_button);
		//continue_button->setLabel(continue_label);

		menu_buttons.add(continue_button);

		Button* credits_button = App->gui->AddButton(w / 2, 180+ (h / 2), buttons, true, &button_idle, &Show_Credits, &button_hover, &button_press);

		Label* credit = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		credit->setString("CREDITS");
		credit->SetParent(credits_button);
		//credits_button->setLabel(credit);

		menu_buttons.add(credits_button);

		Button* settings_button = App->gui->AddButton(w / 2, 240 + (h / 2), buttons, true, &button_idle, /*Anonimous function callback*/[]() {
			App->scene->settings_window->Enable(true);
		}, &button_hover, &button_press);

		Label* settings = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		settings->setString("SETTINGS");
		settings->SetParent(settings_button);

		menu_buttons.add(settings_button);


		Button* exit_button = App->gui->AddButton(w / 2, 300 + (h / 2), buttons, true, &button_idle, /*Anonimous function callback*/[]() {
			App->scene->ret = false;
		}, &button_hover, &button_press);

		Label* exit = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		exit->setString("EXIT");
		exit->SetParent(exit_button);
		//exit_button->setLabel(exit);

		menu_buttons.add(exit_button);
		credits_window = App->gui->AddWindow(w / 2, h / 2, credits_win, false);
		credits_window->SetContentRect(10, 80,0,100);

		settings_window = App->gui->AddWindow(w / 2, h / 2, settings_win, false);


		credits_text = App->gui->AddSprite(0.0f * credits_window->content_rect.w, 0.1f * credits_window->content_rect.h, credits_tex,false);
		credits_text->SetAnchor(0.0f, 0.0f);
		credits_text->SetParent(credits_window);

		Sprite* slider = App->gui->AddSprite(0.9f * credits_window->content_rect.w, credits_window->content_rect.h / 2 + 10, sliders, false, &slider_bar);
		slider->culled = false;
		slider->SetParent(credits_window);
		slider->SetContentRect(0,48);
		
		credits_slider = App->gui->AddSlider(slider->content_rect.w/2, 0, sliders, false, &slider_idle, &Drag_Credits, &slider_hover, &slider_pressed, 0, slider);
		credits_slider->culled = false;
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
	win->SetContentRect(-100, 100);
	Window* win2 = App->gui->AddWindow(0.0f * win->content_rect.w, 0.0f * win->content_rect.h, tex, true, &win_rect);
	win2->SetParent(win);
	win->SetContentRect(50, 50);
	win2->culled = true;
	SDL_Rect idle{ 0, 110, 230, 71 };
	SDL_Rect hovered{ 411, 166, 230, 71 };
	SDL_Rect pressed{ 641, 166, 230, 71 };
	Button* butt = App->gui->AddButton(0.5f * win2->content_rect.w, 0.5f * win2->content_rect.h, tex, true, &idle, [win2]() {
		win2->culled = !win2->culled;
	}, &hovered, &pressed);
	butt->SetParent(win2);
	butt->SetAnchor(0.5f, 0.5f);
	butt->type = InterfaceElement::Interfacetype::TEXT_INPUT;

	Label* text = App->gui->AddLabel(0.5f * butt->content_rect.w, 0.5f * butt->content_rect.h,
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
		if (App->GetFramerateCap() == -1)
			App->SetFramerateCap(60);
		else App->SetFramerateCap(-1);
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
		App->audio->PlayFx(jump_sound);
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

void j1Scene::SumCoin()
{
	coin_lab->setString("X%d", App->entities->player.coins);
}

void button_callback(const char* text) {
	LOG("%s", text);
}

void Game_start()
{
	App->Reload();
	App->scene->state = App->scene->IN_GAME;
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
	App->gui->setFocus(App->scene->credits_window);
	App->scene->credits_bool = true;
}

void Hide_Credits()
{
	App->scene->credits_window->Enable(false);
	App->scene->credits_bool = false;
}

void Drag_Credits()
{
	App->scene->credits_text->rel_pos.y = App->scene->credits_text->initial_pos.y + ((App->scene->credits_slider->initial_pos.y - App->scene->credits_slider->rel_pos.y) * 2.3f);
}