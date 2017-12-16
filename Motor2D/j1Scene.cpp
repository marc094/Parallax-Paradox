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

	button_sound = App->audio->LoadFx("audio/FX/Button.wav");
	if (current_state == INTRO)
	{
		uint w;
		uint h;
		App->win->GetWindowSize(w, h);
		App->entities->active = false;
		logo_back = App->tex->Load("textures/intro background.png");
		logo = App->tex->Load("textures/Marcopolo logo.png");
		Sprite* logo_b_spr = App->gui->AddSprite(w / 2, h / 2, logo_back);
		Sprite* logo_spr = App->gui->AddSprite(w / 2, h / 2, logo);

	
	}
	else if (current_state == IN_GAME)
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


		Sprite* Coin = App->gui->AddSprite(270, h - 50, lifes_sprite, true, &coin_rect);

		coin_lab = App->gui->AddLabel(300, h - 50,40, "gui/Earth 2073.ttf", { 255,255,255,255 }, Label::BLENDED);
		coin_lab->SetAnchor(0.0f, 0.5f);
		coin_lab->setString("X%d", App->entities->player.coins);

		jump_sound = App->audio->LoadFx("audio/FX/Jump.wav");
		change_sound = App->audio->LoadFx("audio/FX/Change2.wav");
		hit_sound = App->audio->LoadFx("audio/FX/Onhit.wav");
		level_sound = App->audio->LoadFx("audio/FX/Wierd.wav");
		coin_sound = App->audio->LoadFx("audio/FX/Coin.wav");


		if (jump_sound == 0 || change_sound == 0 || hit_sound == 0 || level_sound == 0)
			LOG("Error loading sound fx: %s\n", Mix_GetError());

		if (!playing)
		{
			if (level == 1)
				App->audio->PlayMusic("audio/music/MotherEarthBound_Zero_Remix_8_Melodies.ogg", -1);
			else if (level == 2)
				App->audio->PlayMusic("audio/music/Onett_Theme_Remastered_EarthBound.ogg", -1);
			else {
				App->audio->PlayMusic("audio/music/MotherEarthBound_Zero_Remix_8_Melodies.ogg", -1);
			}

			playing = true;
		}

		App->audio->PlayFx(level_sound);

		App->entities->active = true;
	}
	else if (current_state == IN_MENU)
	{
		App->audio->PlayMusic("audio/music/Cant_Help_Falling_In_Love_on_a_Kalimba.ogg", -1);
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
		SDL_Rect slider_pressed = { 33,103,16,94 };
		SDL_Rect slider_hover = { 33,212,16,94 };

		SDL_Rect h_slider_bar = { 54,0,326,7 };
		SDL_Rect h_slider_pressed = { 58,14,10,21 };
		SDL_Rect h_slider_hovered = { 72,14,10,21 };
		SDL_Rect h_slider_idle = { 86,14,10,21 };

		uint w;
		uint h;
		App->win->GetWindowSize(w, h);

		SDL_Rect button_idle = { 1000,1000,250,58 };
		SDL_Rect button_hover = { 0,0,250,58 };
		SDL_Rect button_press = { 0,58,250,58 };

		App->gui->AddSprite( w/2, h/2, menu_background);
		Sprite* title_spr = App->gui->AddSprite(w / 2, (h / 2 - 100), title);

		//START BUTTON
		Button* start_button = App->gui->AddButton((w / 2), 60 + (h / 2), buttons, true, &button_idle, &Game_start, &button_hover, &button_press);
		
		Label* start = App->gui->AddLabel(start_button->content_rect.w/2, (start_button->content_rect.h/2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		start->setString("START");
		start->SetParent(start_button);

		menu_buttons.add(start_button);


		//CONTINUE BUTTON
		Button* continue_button = App->gui->AddButton(w / 2, 120+ (h / 2), buttons, true, &button_idle, &Game_continue, &button_hover, &button_press);

		Label* continue_label = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		continue_label->setString("CONTINUE");
		continue_label->SetParent(continue_button);

		menu_buttons.add(continue_button);


		//CREDITS BUTTON
		Button* credits_button = App->gui->AddButton(w / 2, 180+ (h / 2), buttons, true, &button_idle, &Show_Credits, &button_hover, &button_press);

		Label* credit = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		credit->setString("CREDITS");
		credit->SetParent(credits_button);

		menu_buttons.add(credits_button);


		//SETTINGS BUTTON
		Button* settings_button = App->gui->AddButton(w / 2, 240 + (h / 2), buttons, true, &button_idle, &ShowSettings, &button_hover, &button_press);

		Label* settings = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		settings->setString("SETTINGS");
		settings->SetParent(settings_button);

		menu_buttons.add(settings_button);

		
		//EXIT BUTTON
		Button* exit_button = App->gui->AddButton(w / 2, 300 + (h / 2), buttons, true, &button_idle, &exit, &button_hover, &button_press);

		Label* exit = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		exit->setString("EXIT");
		exit->SetParent(exit_button);

		menu_buttons.add(exit_button);


		//CREDITS WINDOW
		credits_window = App->gui->AddWindow(w / 2, h / 2, credits_win, false);
		credits_window->SetContentRect(10, 87, 0, 15);
		credits_window->locked = true;

		credits_text = App->gui->AddSprite(0, 0, credits_tex);
		credits_text->SetAnchor(0.0f, 0.0f);
		credits_text->SetParent(credits_window);

		Sprite* slider = App->gui->AddSprite((int)(0.97f * credits_window->content_rect.w), credits_window->content_rect.h / 2 - 20, sliders, true, &slider_bar);
		slider->culled = false;
		slider->SetParent(credits_window);
		slider->SetContentRect(0, 48, 0, 48);
		
		credits_slider = App->gui->AddSlider(slider->content_rect.w/2, 0, sliders, true, &slider_idle, &Drag_Credits, &slider_hover, &slider_pressed, 0, slider);
		credits_slider->culled = false;

		//SETTINGS WINDOW
		settings_window = App->gui->AddWindow(w / 2, h / 2, settings_win, false);
		settings_window->SetContentRect(10, 40, 10, 60);
		settings_window->locked = true;

		Label* music_set_lab = App->gui->AddLabel(15, 80, 30, "gui/Earth 2073.ttf", { 255,255,255,255 });
		music_set_lab->SetParent(settings_window);
		music_set_lab->SetAnchor(0, 0.5f);
		music_set_lab->setString("Music:");

		Sprite* music_set_slid = App->gui->AddSprite(settings_window->content_rect.w / 2, 80, sliders, true, &h_slider_bar);
		music_set_slid->SetParent(settings_window);
		music_set_slid->SetAnchor(0.5f, 0.0f);
		music_set_slid->SetContentRect(5, 0, 5, 0);
		music_set_slid->culled = false;

		set_mus_slider = App->gui->AddSlider(music_set_slid->content_rect.w / 2, 0, sliders, true, &h_slider_idle, &SetVolumeMusic, &h_slider_hovered, &h_slider_pressed, 1, music_set_slid);
		set_mus_slider->culled = false;
		set_mus_slider->rel_pos.x = (App->audio->GetVolumeMusic() * music_set_slid->content_rect.w) + set_mus_slider->initial_pos.x - music_set_slid->content_rect.w * music_set_slid->GetAnchorX();

		Label* fx_set_lab = App->gui->AddLabel(15, 160, 30, "gui/Earth 2073.ttf", { 255,255,255,255 });
		fx_set_lab->SetParent(settings_window);
		fx_set_lab->SetAnchor(0, 0.5f);
		fx_set_lab->setString("SFX:");

		Sprite* fx_set_slid = App->gui->AddSprite(0.5f * settings_window->content_rect.w, 160, sliders, true, &h_slider_bar);
		fx_set_slid->SetParent(settings_window);
		fx_set_slid->SetAnchor(0.5f, 0.0f);
		fx_set_slid->SetContentRect(5, 0, 5, 0);
		fx_set_slid->culled = false;

		Slider* set_fx_slider = App->gui->AddSlider(fx_set_slid->content_rect.w / 2, 0, sliders, true, &h_slider_idle, &SetVolumeFX, &h_slider_hovered, &h_slider_pressed, 1, fx_set_slid);
		set_fx_slider->culled = false;
		set_fx_slider->rel_pos.x = (App->audio->GetVolumeFX() * fx_set_slid->content_rect.w) + set_fx_slider->initial_pos.x - fx_set_slid->content_rect.w * fx_set_slid->GetAnchorX();

		Label* fps_set_lab = App->gui->AddLabel(15, 240, 30, "gui/Earth 2073.ttf", { 255,255,255,255 });
		fps_set_lab->SetParent(settings_window);
		fps_set_lab->SetAnchor(0, 0.5f);
		fps_set_lab->setString("Framerate:");

		Button* fps_button_30 = App->gui->AddButton(0.2f * settings_window->content_rect.w, 290, buttons, true, &button_idle, &Fps_30, &button_hover, &button_press);
		fps_button_30->SetParent(settings_window);

		Label* fps_label_30 = App->gui->AddLabel(0.5f * fps_button_30->content_rect.w, 0.5f * fps_button_30->content_rect.h, 33, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
		fps_label_30->setString("30 fps");
		fps_label_30->SetParent(fps_button_30);

		Button* fps_button_60 = App->gui->AddButton(0.5f * settings_window->content_rect.w, 290, buttons, true, &button_idle, &Fps_60, &button_hover, &button_press);
		fps_button_60->SetParent(settings_window);

		Label* fps_label_60 = App->gui->AddLabel(0.5f * fps_button_60->content_rect.w, 0.5f * fps_button_60->content_rect.h, 33, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
		fps_label_60->setString("60 fps");
		fps_label_60->SetParent(fps_button_60);

		Button* fps_button_Uncapped = App->gui->AddButton(0.8f * settings_window->content_rect.w, 290, buttons, true, &button_idle, &Fps_Uncapped, &button_hover, &button_press);
		fps_button_Uncapped->SetParent(settings_window);

		Label* fps_label_Uncapped = App->gui->AddLabel(0.5f * fps_button_Uncapped->content_rect.w, 0.5f * fps_button_Uncapped->content_rect.h, 33, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
		fps_label_Uncapped->setString("Uncapped");
		fps_label_Uncapped->SetParent(fps_button_Uncapped);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (credits_bool && credits_window->enabled)
			Hide_Credits(0);
		else if (settings_bool && settings_window->enabled)
			Hide_Settings(0);
		else
			ret = false;
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

	if (current_state == IN_GAME)
		CheckEnd();
	else if (current_state == INTRO)
	{
		if (intro_time.Count(4))
		{
			App->Reload();
			state = IN_MENU;
		}
	}

	else if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->Reload();
		state = IN_GAME;
	}
	return ret;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (current_state == IN_GAME && !App->entities->player.hit)
	{
		CheckInput(dt);
		App->map->Draw();
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
	current_state = state;
	return true;
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
		App->entities->player.SwapLayer();

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->entities->player.Accelerate(0, ACCELERATION, dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->entities->player.Accelerate(-ACCELERATION, 0, dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
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
	if (App->entities->player.GetPosition().DistanceTo(App->map->GetFinalPlayerPos()) < 25)
		App->scene->ChangeScene(level + 1);
}


bool j1Scene::Load(pugi::xml_node& data) 
{
	level = data.child("level").attribute("current_level").as_int();
	time.loaded_time = data.child("time").attribute("sec").as_int();
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("level");

	pos.append_attribute("current_level") = level;

	pugi::xml_node time_node = data.append_child("time");
	time_node.append_attribute("sec") = time.Read();

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

void Game_start(int, ...)
{
	App->Reload();
	App->scene->state = App->scene->IN_GAME;

}

void Game_continue(int, ...)
{
	App->LoadGame();
	App->Reload();
	App->scene->state = App->scene->IN_GAME;
}

void Show_Credits(int, ...)
{
	App->scene->credits_window->Enable(true);
	App->gui->setFocus(App->scene->credits_window);
	App->scene->credits_bool = true;
}

void Hide_Credits(int, ...)
{
	App->scene->credits_window->Enable(false);
	App->scene->credits_bool = false;
}
void Hide_Settings(int, ...)
{
	App->scene->settings_window->Enable(false);
	App->scene->settings_bool = false;
}

void Drag_Credits(int, ...)
{
	App->scene->credits_text->rel_pos.y = App->scene->credits_text->initial_pos.y + (int)((App->scene->credits_slider->initial_pos.y - App->scene->credits_slider->rel_pos.y) * 2.3f);
}

void exit(int, ...)
{
	App->scene->ret = false;
}

void ShowSettings(int, ...) {
	App->scene->settings_window->Enable(true);
	App->scene->settings_bool = true;
}

void SetVolumeFX(int n, ...) {
	va_list  ap;
	va_start(ap, n);
	double percent = va_arg(ap, double);
	va_end(ap);

	App->audio->SetVolumeFX((float)percent);
}

void SetVolumeMusic(int n, ...) {
	va_list  ap;
	va_start(ap, n);
	double percent = va_arg(ap, double);
	va_end(ap);

	App->audio->SetVolumeMusic((float)percent);
}

void Fps_30(int, ...)
{
	App->SetFramerateCap(30);
}

void Fps_60(int, ...)
{
	App->SetFramerateCap(60);
}

void Fps_Uncapped(int, ...)
{
	App->SetFramerateCap(-1);
}
