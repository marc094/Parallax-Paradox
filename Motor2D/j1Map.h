#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

enum ColliderType {
	COLLIDER_PLAYER = 1,
	COLLIDER_FRONT_LAYER,
	COLLIDER_BACK_LAYER,
};
struct Collider
{
	SDL_Rect rect;
	ColliderType collidertype;

};

// TODO 1: Create a struct for the map layer
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

	// TODO 6: Short function to get the value of x,y
//iPoint getWorldPositionFromMap(int x, int y, uint tile_width);


// ----------------------------------------------------
struct TileSet
{
	// TODO 7: Create a method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
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

	// TODO 2: Add a list/array of layers to the map!
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

	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;

	fPoint GetInitialPlayerPos() const;



private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	// TODO 3: Create a method that loads a single layer
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);

public:

	MapData data;
	int		gravity;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	fPoint				initial_player_pos;
	SDL_Texture* background;
};

#endif // __j1MAP_H__