#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "Brofiler\Brofiler.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true))
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = (float)App->win->screen_surface->w;
		camera.h = (float)App->win->screen_surface->h;
		camera.x = 0.0f;
		camera.y = 0.0f;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);
	return true;
}

bool j1Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp(pugi::xml_node&)
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_float();
	camera.y = data.child("camera").attribute("y").as_float();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

SDL_Texture* j1Render::Snapshot()
{
	SDL_Surface* surf = SDL_CreateRGBSurface(0, (int)camera.w, (int)camera.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_ARGB8888, surf->pixels, surf->pitch);
	SDL_Texture* ret = SDL_CreateTextureFromSurface(renderer, surf);//App->tex->LoadSurface(surf);
	SDL_FreeSurface(surf);
	return ret;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivot_x, int pivot_y, bool h_flip,bool use_camera, SDL_Color color, float _scale) const
{
	bool ret = true;

	float scale = App->win->GetScale();


	SDL_Rect rect;

	rect.x = (int)(x * scale);
	rect.y = (int)(y * scale);



	if (use_camera)
	{
		rect.x -= (int)(camera.x * speed);
		rect.y -= (int)(camera.y * speed);
	}


	if (pivot_y < 0)
	{
		rect.y += pivot_y;
	}

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	if (_scale == 0) {
		rect.w = (int)(rect.w * scale);
		rect.h = (int)(rect.h * scale);
	}
	else
	{
		rect.w = (int)(rect.w * _scale);
		rect.h = (int)(rect.h * _scale);
	}


	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (h_flip)
		flip = SDL_FLIP_HORIZONTAL;

	if (color.r != 255 && color.g != 255 && color.b != 255)
	{
		SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	if (color.r != 255 || color.g != 255 || color.b != 255)
	{
		SDL_SetTextureColorMod(texture, 255, 255, 255);
	}
	return ret;
}

bool j1Render::BlitGui(SDL_Texture * texture, int x, int y, const SDL_Rect * section, bool use_camera, float speed, double angle, bool h_flip, SDL_Color color, int pivot_x, int pivot_y)
{
	bool ret = true;

	float scale = 1.f;//App->win->GetScale();
	SDL_Rect rect;
	if (use_camera)
	{
		rect.x = (int)(x * scale) - (int)(camera.x * speed);
		rect.y = (int)(y * scale) - (int)(camera.y * speed);
	}
	else
	{
		rect.x = (int)(x * scale);
		rect.y = (int)(y * scale);
	}

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w = (int)(rect.w * scale);
	rect.h = (int)(rect.h * scale);

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (h_flip)
		flip = SDL_FLIP_HORIZONTAL;

	if (color.r != 255 && color.g != 255 && color.b != 255)
		SDL_SetTextureColorMod(texture, color.r, color.g, color.b);

	if (color.a != 255)
		SDL_SetTextureAlphaMod(texture, color.a);

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	if (color.r != 255 || color.g != 255 || color.b != 255)
		SDL_SetTextureColorMod(texture, 255, 255, 255);

	if (color.a != 255)
		SDL_SetTextureAlphaMod(texture, 255);

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float speed, bool filled, bool use_camera, bool scaled) const
{
	bool ret = true;
	float scale = (scaled) ? App->win->GetScale() : 1.f;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(rect.x * scale) - (int)(camera.x * speed);
		rec.y = (int)(rect.y * scale) - (int)(camera.y * speed);
		rec.w = (int)(rec.w * scale);
		rec.h = (int)(rec.h * scale);
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera, bool scaled) const
{
	bool ret = true;
	float scale = (scaled) ? App->win->GetScale() : 1.f;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, (int)(x1 * scale - camera.x), (int)(y1 * scale - camera.y), (int)(x2 * scale - camera.x), (int)(y2 * scale - camera.y));
	else
		result = SDL_RenderDrawLine(renderer, (int)(x1 * scale), (int)(y1 * scale), (int)(x2 * scale), (int)(y2 * scale));

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera, bool scaled) const
{
	bool ret = true;
	float scale = (scaled) ? App->win->GetScale() : 1.f;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

