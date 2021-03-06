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
#include "j1Transition.h"

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
	App->SetTimeScale(1.f);
	if (level > max_level)
		level = 1;

	button_sound = App->audio->LoadFx("audio/FX/Button.wav");
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
	uiPoint dims = App->gui->GetGuiSize();
	w = dims.x;
	h = dims.y;

	SDL_Rect button_idle = { 1000,1000,250,58 };
	SDL_Rect button_hover = { 0,0,250,58 };
	SDL_Rect button_press = { 0,58,250,58 };
	if (current_state == END) {
		App->entities->active = false;
		uiPoint dims = App->gui->GetGuiSize();
		Label* win_label = App->gui->AddLabel(dims.x / 2, dims.y / 2 - dims.y / 12, 60, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
		win_label->setString("CONGRATULATIONS");
		win_label->ComputeRects();

		App->transition->MakeTransition(&DoLoadMenu, j1Transition::FADE_TO_BLACK, 10.f);
	}
	else if (current_state == INTRO)
	{
		App->entities->active = false;
		logo_back = App->tex->Load("textures/intro background.png");
		logo = App->tex->Load("textures/Marcopolo logo.png");
		Sprite* logo_b_spr = App->gui->AddSprite(w / 2, h / 2, logo_back);
		Sprite* logo_spr = App->gui->AddSprite(w / 2, h / 2, logo);

		App->transition->MakeTransition(nullptr, j1Transition::FADE_TO_BLACK, 2.5f);
		App->transition->fade_state = j1Transition::FADING_IN;
	}
	else if (current_state == IN_GAME)
	{
		App->map->Load(xml_file_name[level - 1].GetString());
		lifes_sprite = App->tex->Load("textures/Lifeicon.png");
		SDL_Rect life = { 0,0,45,48 };
		SDL_Rect coin_rect = { 45,0,36,48 };
		Sprite* first_life = App->gui->AddSprite(50, h - 50, lifes_sprite, life);

		Sprite* second_life = App->gui->AddSprite(110, h - 50, lifes_sprite, life);

		Sprite* third_life = App->gui->AddSprite(170, h - 50, lifes_sprite, life);

		lives.add(first_life);
		lives.add(second_life);
		lives.add(third_life);

		time_lab = App->gui->AddLabel(w - 150, h  - 50, 40, "gui/Earth 2073.ttf", { 255,255,255,255 },Label::BLENDED);
		time_lab->SetAnchor(0.0f, 0.5f);


		Sprite* Coin = App->gui->AddSprite(270, h - 50, lifes_sprite, coin_rect);

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
			App->audio->PlayMusic("audio/music/InGame.ogg", -1);
			playing = true;
		}

		App->audio->PlayFx(level_sound);

		App->entities->active = true;

		switch (level)
		{
		case 1:
		{
			App->entities->Add_Coin({ 476, 678 });
			App->entities->Add_Coin({ 416, 894 });
			App->entities->Add_Coin({ 815, 561 });
			App->entities->Add_Coin({ 481, 47 });
			firstlevel_lab = App->gui->AddLabel(w /2, h /3, 35, "gui/Earth 2073.ttf", { 255,255,255,255 }, Label::BLENDED);
			firstlevel_lab->setString("Press LB to switch layer");
			firstlevel_lab->Enable(false);
			break;
		}
		case 2:
		{
			App->entities->Add_Coin({ 1474, 856 });
			App->entities->Add_Coin({ 900, 840 });
			App->entities->Add_Coin({ 400, 1000 });
			App->entities->Add_Coin({ 1000, 600 });
			App->entities->Add_Coin({ 1480, 585 });
			App->entities->Add_Coin({ 1185, 216 });
			break;
		}
		case 3:
		{
			App->entities->Add_Coin({ 891, 973 });
			App->entities->Add_Coin({ 1296, 854 });
			App->entities->Add_Coin({ 762,600 });
			App->entities->Add_Coin({ 730,730 });
			break;
		}
		case 4:
		{
			App->entities->Add_Coin({ 998,706 });
			App->entities->Add_Coin({ 621, 548 });
			App->entities->Add_Coin({ 1332,421 });
			App->entities->Add_Coin({ 171,134 });
			break;
		}
		}
	}
	else if (current_state == IN_MENU)
	{
		//Estaria guay tenir musica al menu
		App->audio->PlayMusic("audio/music/Intro.ogg", -1);
		App->entities->active = false;
		menu_background = App->tex->Load("textures/menu background.png");
		title = App->tex->Load("textures/title.png");

		App->gui->AddSprite( w/2, h/2, menu_background);
		Sprite* title_spr = App->gui->AddSprite(w / 2, (h / 2 - 100), title);

		//START BUTTON
		Button* start_button = App->gui->AddButton((w / 2), 60 + (h / 2), buttons, button_idle, true, &Game_start, button_hover, button_press);
		
		Label* start = App->gui->AddLabel(start_button->content_rect.w/2, (start_button->content_rect.h/2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		start->setString("START");
		start->SetParent(start_button);

		menu_buttons.add(start_button);


		//CONTINUE BUTTON
		Button* continue_button = App->gui->AddButton(w / 2, 120+ (h / 2), buttons, button_idle, true, &Game_continue, button_hover, button_press);

		Label* continue_label = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		continue_label->setString("CONTINUE");
		continue_label->SetParent(continue_button);

		menu_buttons.add(continue_button);


		//CREDITS BUTTON
		Button* credits_button = App->gui->AddButton(w / 2, 180+ (h / 2), buttons, button_idle, true, &Show_Credits, button_hover, button_press);

		Label* credit = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		credit->setString("CREDITS");
		credit->SetParent(credits_button);

		menu_buttons.add(credits_button);


		//SETTINGS BUTTON
		Button* settings_button = App->gui->AddButton(w / 2, 240 + (h / 2), buttons, button_idle, true, &ShowSettings, button_hover, button_press);

		Label* settings = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		settings->setString("SETTINGS");
		settings->SetParent(settings_button);

		menu_buttons.add(settings_button);

		
		//EXIT BUTTON
		Button* exit_button = App->gui->AddButton(w / 2, 300 + (h / 2), buttons, button_idle, true, &exit, button_hover, button_press);

		Label* exit = App->gui->AddLabel(start_button->content_rect.w / 2, (start_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		exit->setString("EXIT");
		exit->SetParent(exit_button);

		menu_buttons.add(exit_button);


		//CREDITS WINDOW
		Window_Info w_info_credits;
		w_info_credits.x = w / 2;
		w_info_credits.y = h / 2;
		w_info_credits.content_rect_margins = { 10, 87, 0, 15 };
		w_info_credits.tex = credits_win;
		w_info_credits.OnClose = &Hide_Credits;
		w_info_credits.enabled = false;

		credits_window = App->gui->AddWindow(w_info_credits);

		credits_text = App->gui->AddSprite(0, 0, credits_tex);
		credits_text->SetAnchor(0.0f, 0.0f);
		credits_text->SetParent(credits_window);

		Sprite* slider = App->gui->AddSprite((int)(0.97f * credits_window->content_rect.w), credits_window->content_rect.h / 2 - 20, sliders, slider_bar);
		slider->culled = false;
		slider->SetParent(credits_window);
		slider->SetContentRect(0, 48, 0, 48);
		
		credits_slider = App->gui->AddSlider(slider->content_rect.w/2, 0, sliders, slider_idle, true, Drag_Credits, slider_hover, slider_pressed, 0, slider);
		credits_slider->culled = false;
	}

	//SETTINGS WINDOW
	settings_window = CreateSettingsWindow(w / 2, h / 2, h_slider_bar, h_slider_idle, h_slider_hovered, h_slider_pressed, button_idle, button_hover, button_press);

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
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (state == IN_GAME) {
			if (!settings_bool && !settings_window->isEnabled()) {
				ShowSettings(0);
			}
			else Hide_Settings(0);
		}
		else
			ret = false;
	}

	if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_START) == KEY_DOWN) {
		if (state == IN_GAME) {
			if (!settings_bool && !settings_window->isEnabled()) {
				ShowSettings(0);
			}
			else Hide_Settings(0);
		}
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
			App->transition->MakeTransition(&DoReload, j1Transition::FADE_TO_BLACK, 2.5f);
			state = IN_MENU;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->transition->MakeTransition(&DoLoadInGame, j1Transition::FADE_TO_BLACK, 2.5f);
	}

	if (current_state == IN_GAME)
	{
		if (!App->entities->player.hit)
		{
			CheckInput(dt);
		}
		
		if (level == 1 )
		{
			iRect player_rect = App->entities->player.collider;
			player_rect.x = (int)App->entities->player.GetPosition().x;
			player_rect.y = (int)App->entities->player.GetPosition().y;

			if (App->entities->player.current_layer == FRONT_LAYER && App->collision->DoCollide(player_rect.toSDL_Rect(), { 1143,612,123,27 }))
				firstlevel_lab->Enable(true);
			else
				firstlevel_lab->Enable(false);
		}
		App->map->Draw();
		time += dt;
		time_lab->setString("%.2f", time);
	}

	// "Map:%dx%d Tiles:%dx%d Tilesets:%d"
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	//App->win->SetTitle(title.GetString());
	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	current_state = state;
	return true;
}

// Called before quitting
bool j1Scene::CleanUp(pugi::xml_node& config)
{
	LOG("Freeing scene");

	xml_file_name.Clear();

	return true;
}

void DoReload(int, ...) {
	App->Reload();
	App->scene->transitioninig = false;
}

void DoLoadInGame(int, ...) {
	App->Reload();
	App->scene->state = j1Scene::IN_GAME;
	App->scene->transitioninig = false;
}

void DoLoadMenu(int, ...) {
	App->Reload();
	App->scene->state = j1Scene::IN_MENU;
	App->scene->transitioninig = false;
}

void DoLoadEnd(int, ...) {
	App->Reload();
	App->scene->state = j1Scene::END;
	App->scene->transitioninig = false;
}

void j1Scene::ChangeScene(uint _level) {
	level = _level;
	float trans_time = 3.f;
	if (level > max_level) {
		level = 1;
		trans_time = 5.f;
		playing = false;
		App->audio->StopMusic(trans_time);
		App->transition->MakeTransition(&DoLoadEnd, j1Transition::FADE_TO_BLACK, trans_time);
	}
	else App->transition->MakeTransition(&DoReload, j1Transition::FADE_TO_BLACK, trans_time);
	App->SetTimeScaleTo(0.1f, 2.f);
}

void j1Scene::CheckInput(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN || App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == KEY_DOWN)
		App->entities->player.SwapLayer();

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT || App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY, +1))
		App->entities->player.Accelerate(0, ACCELERATION, dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT || App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX, -1))
		App->entities->player.Accelerate(-ACCELERATION, 0, dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT || App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX, +1))
		App->entities->player.Accelerate(ACCELERATION, 0, dt);

	if ((App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_DOWN
		|| App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN) && !App->entities->player.isJumping() && !App->entities->player.god_mode)
	{
		App->entities->player.setJumping(true);
		App->entities->player.Accelerate(0, -JUMP_FORCE, dt);
		App->audio->PlayFx(jump_sound);
	}
	else if ((App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT || App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT
		|| App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY, -1)) && App->entities->player.god_mode)
	{
		App->entities->player.Accelerate(0, -ACCELERATION, dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		App->entities->player.god_mode = !App->entities->player.god_mode;
}

void j1Scene::CheckEnd() {
	//App->render->DrawCircle(App->map->GetFinalPlayerPos().x, App->map->GetFinalPlayerPos().y, 50, 255, 255, 255, 255, true, false);
	if (App->entities->player.GetPosition().DistanceTo(App->map->GetFinalPlayerPos() + fPoint(15.f, 0.f)) < 50.f && !transitioninig) {
		transitioninig = true;
		App->scene->ChangeScene(level + 1);
	}
}


bool j1Scene::Load(pugi::xml_node& data) 
{
	level = data.child("level").attribute("current_level").as_int();
	time = data.child("time").attribute("sec").as_float();
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("level");

	pos.append_attribute("current_level") = level;

	pugi::xml_node time_node = data.append_child("time");
	time_node.append_attribute("sec") = time;

	return true;
}

void j1Scene::ChangeLifes()
{
	p2List_item<Sprite*>* curr = lives.end;
	while (curr->prev != NULL)
	{
		if (curr->data->isEnabled())
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


Window* j1Scene::CreateSettingsWindow(int x, int y, SDL_Rect h_slider_bar, SDL_Rect h_slider_idle, SDL_Rect h_slider_hovered, SDL_Rect h_slider_pressed, SDL_Rect button_idle, SDL_Rect button_hover, SDL_Rect button_press)
{
	Window_Info w_info;
	w_info.x = x;
	w_info.y = y;
	w_info.content_rect_margins = { 10, 40, 10, 60 };
	w_info.tex = settings_win;
	w_info.enabled = false;
	w_info.OnClose = &Hide_Settings;

	Window* settings_window = App->gui->AddWindow(w_info);

	Label* music_set_lab = App->gui->AddLabel(15, 80, 30, "gui/Earth 2073.ttf", { 255,255,255,255 });
	music_set_lab->SetParent(settings_window);
	music_set_lab->SetAnchor(0, 0.5f);
	music_set_lab->setString("Music:");

	Sprite* music_set_slid = App->gui->AddSprite(settings_window->content_rect.w / 2, 80, sliders, h_slider_bar);
	music_set_slid->SetParent(settings_window);
	music_set_slid->SetAnchor(0.5f, 0.0f);
	music_set_slid->SetContentRect(5, 0, 5, 0);
	music_set_slid->culled = false;

	Slider* set_mus_slider = App->gui->AddSlider(music_set_slid->content_rect.w / 2, 0, sliders, h_slider_idle, true, &SetVolumeMusic, h_slider_hovered, h_slider_pressed, 1, music_set_slid);
	set_mus_slider->culled = false;
	set_mus_slider->rel_pos.x = (App->audio->GetVolumeMusic() * music_set_slid->content_rect.w) + set_mus_slider->initial_pos.x - music_set_slid->content_rect.w * music_set_slid->GetAnchorX();

	Label* fx_set_lab = App->gui->AddLabel(15, 160, 30, "gui/Earth 2073.ttf", { 255,255,255,255 });
	fx_set_lab->SetParent(settings_window);
	fx_set_lab->SetAnchor(0, 0.5f);
	fx_set_lab->setString("SFX:");

	Sprite* fx_set_slid = App->gui->AddSprite(0.5f * settings_window->content_rect.w, 160, sliders, h_slider_bar);
	fx_set_slid->SetParent(settings_window);
	fx_set_slid->SetAnchor(0.5f, 0.0f);
	fx_set_slid->SetContentRect(5, 0, 5, 0);
	fx_set_slid->culled = false;

	Slider* set_fx_slider = App->gui->AddSlider(fx_set_slid->content_rect.w / 2, 0, sliders, h_slider_idle, true, &SetVolumeFX, h_slider_hovered, h_slider_pressed, 1, fx_set_slid);
	set_fx_slider->culled = false;
	set_fx_slider->rel_pos.x = (App->audio->GetVolumeFX() * fx_set_slid->content_rect.w) + set_fx_slider->initial_pos.x - fx_set_slid->content_rect.w * fx_set_slid->GetAnchorX();

	Label* fps_set_lab = App->gui->AddLabel(15, 240, 30, "gui/Earth 2073.ttf", { 255,255,255,255 });
	fps_set_lab->SetParent(settings_window);
	fps_set_lab->SetAnchor(0, 0.5f);
	fps_set_lab->setString("Framerate:");

	Button* fps_button_30 = App->gui->AddButton(0.2f * settings_window->content_rect.w, 290, buttons, button_idle, true, &Fps_30, button_hover, button_press);
	fps_button_30->SetParent(settings_window);

	Label* fps_label_30 = App->gui->AddLabel(0.5f * fps_button_30->content_rect.w, 0.5f * fps_button_30->content_rect.h, 33, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
	fps_label_30->setString("30 fps");
	fps_label_30->SetParent(fps_button_30);

	Button* fps_button_60 = App->gui->AddButton(0.5f * settings_window->content_rect.w, 290, buttons, button_idle, true, &Fps_60, button_hover, button_press);
	fps_button_60->SetParent(settings_window);

	Label* fps_label_60 = App->gui->AddLabel(0.5f * fps_button_60->content_rect.w, 0.5f * fps_button_60->content_rect.h, 33, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
	fps_label_60->setString("60 fps");
	fps_label_60->SetParent(fps_button_60);

	Button* fps_button_Uncapped = App->gui->AddButton(0.8f * settings_window->content_rect.w, 290, buttons, button_idle, true, &Fps_Uncapped, button_hover, button_press);
	fps_button_Uncapped->SetParent(settings_window);

	Label* fps_label_Uncapped = App->gui->AddLabel(0.5f * fps_button_Uncapped->content_rect.w, 0.5f * fps_button_Uncapped->content_rect.h, 33, "gui/Earth 2073.ttf", { 255, 255, 255, 255 });
	fps_label_Uncapped->setString("Uncapped");
	fps_label_Uncapped->SetParent(fps_button_Uncapped);

	if (state == IN_GAME) {
		Button* exit_button = App->gui->AddButton(0.75f * settings_window->content_rect.w, 360, buttons, button_idle, true, &exit, button_hover, button_press);

		Label* exit_label = App->gui->AddLabel(exit_button->content_rect.w / 2, (exit_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		exit_label->setString("EXIT");
		exit_label->SetParent(exit_button);

		exit_button->SetParent(settings_window);

		Button* menu_button = App->gui->AddButton(0.25f * settings_window->content_rect.w, 360, buttons, button_idle, true, &toMenu, button_hover, button_press);

		Label* menu_label = App->gui->AddLabel(menu_button->content_rect.w / 2, (menu_button->content_rect.h / 2), 33, "gui/Earth 2073.ttf", { 255,255,255,255 });
		menu_label->setString("To Menu");
		menu_label->SetParent(menu_button);

		menu_button->SetParent(settings_window);
	}

	return settings_window;
}


void button_callback(const char* text) {
	LOG("%s", text);
}

void StartAlready(int, ...) {
	App->Reload();
	App->scene->time = 0.f;
	App->scene->state = App->scene->IN_GAME;
}

void Game_start(int, ...)
{
	App->transition->MakeTransition(&StartAlready, j1Transition::FADE_TO_BLACK);
}

void ContinueAlready(int, ...) {
	App->LoadGame();
	App->Reload();
	App->scene->time = 0.f;
	App->scene->state = App->scene->IN_GAME;
}

void Game_continue(int, ...)
{
	App->transition->MakeTransition(&ContinueAlready, j1Transition::FADE_TO_BLACK);
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
	App->SetTimeScale(1.0f);
}

void Drag_Credits(int, ...)
{
	App->scene->credits_text->rel_pos.y = App->scene->credits_text->initial_pos.y + (int)((App->scene->credits_slider->initial_pos.y - App->scene->credits_slider->rel_pos.y) * 2.3f);
}

void exit(int, ...)
{
	App->scene->ret = false;
}

void toMenu(int, ...)
{
	App->transition->MakeTransition(&DoLoadMenu, j1Transition::FADE_TO_BLACK);
}

void ShowSettings(int, ...) {
	App->scene->settings_window->Enable(true);
	App->scene->settings_bool = true;
	App->SetTimeScale(0.0f);
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