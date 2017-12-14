#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Collision.h"

struct SDL_Texture;

struct MapLayer {
	uint*			tiles = nullptr;
	p2SString		name = nullptr;
	uint			width = 0;
	uint			height = 0;
	uint			size = 0;
	float			parallax_speed = 1.0f;
	LayerID			layer;

	p2List<Collider*>   layer_colliders;

	~MapLayer() {
		free(tiles);
		p2List_item<Collider*>* item;
		item = layer_colliders.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}
		layer_colliders.clear();
	}

};

// ----------------------------------------------------

	
//iPoint getWorldPositionFromMap(int x, int y, uint tile_width);


// ----------------------------------------------------
struct TileSet
{
	
	SDL_Rect GetTileRect(int id) const;

	p2SString			name = nullptr;
	int					firstgid = 0;
	int					margin = 0;
	int					spacing = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Texture*		texture = nullptr;
	int					tex_width = 0;
	int					tex_height = 0;
	int					num_tiles_width = 0;
	int					num_tiles_height = 0;
	int					offset_x = 0;
	int					offset_y = 0;

	~TileSet();
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width = 0;
	int					height = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Color			background_color;
	MapTypes			type = MAPTYPE_UNKNOWN;
	p2List<TileSet*>	tilesets;

	
	p2List<MapLayer*>	layers;

	~MapData() {
		p2List_item<TileSet*>* item;
		item = tilesets.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}
		tilesets.clear();

		p2List_item<MapLayer*>* _item;
		_item = layers.start;

		while (_item != NULL)
		{
			RELEASE(_item->data);
			_item = _item->next;
		}
		layers.clear();
	}
};


// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);


	iPoint MapToWorld(int x, int y) const;
	iPoint MapToWorld(const iPoint p) const;

	iPoint WorldToMap(int x, int y) const;
	iPoint WorldToMap(const iPoint p) const;

	fPoint GetInitialPlayerPos() const;

	fPoint GetFinalPlayerPos() const {
		return final_pos;
	}

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);

public:

	MapData				data;
	int					gravity;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded = false;
	fPoint				initial_player_pos;
	fPoint				final_pos;
	SDL_Texture*		background = nullptr;
};

#endif // __j1MAP_H__