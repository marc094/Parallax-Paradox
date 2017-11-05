#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

enum ColliderType {
	COLLIDER_PLAYER = 1,
	COLLIDER_BACK_LAYER,
	COLLIDER_FRONT_LAYER,
	COLLIDER_ENEMY,
	COLLIDER_NONE
};
struct Collider
{
	SDL_Rect rect;
	ColliderType collidertype;

};


struct MapLayer {
	uint*			tiles = nullptr;
	p2SString		name = nullptr;
	uint			width;
	uint			height;
	uint			size;
	float			parallax_speed = 1.0f;

	p2List<Collider*>   layer_colliders;

	~MapLayer() {
		RELEASE(tiles);
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

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;

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
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
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

	MapData data;
	int		gravity;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	fPoint				initial_player_pos;
	fPoint				final_pos;
	SDL_Texture*		background;
};

#endif // __j1MAP_H__