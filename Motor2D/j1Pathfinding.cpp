#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Map.h"
#include "j1Render.h"

j1PathFinding::j1PathFinding() : j1Module(), last_path(nullptr), width(0), height(0)
{
	name.create("pathfinding");
	maps.clear();
	maps_ground.clear();

	for (uint i = 0; i < LayerID::LAYER_AMOUNT; i++) {
		maps.add(*new p2DynArray<uint>());
		maps_ground.add(*new p2DynArray<uint>());
	}
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	for (p2List_item<p2DynArray<uint>>* map = maps.start; map != nullptr; map = map->next)
		map->data.Clear();
	maps.clear();

	for (p2List_item<p2DynArray<uint>>* map = maps_ground.start; map != nullptr; map = map->next)
		map->data.Clear();
	maps_ground.clear();
}

// Called before quitting
bool j1PathFinding::CleanUp(pugi::xml_node&)
{
	LOG("Freeing pathfinding library");
	
	for (p2List_item<p2DynArray<uint>>* map = maps.start; map != nullptr; map = map->next)
		map->data.Clear();
	maps.clear();

	for (p2List_item<p2DynArray<uint>>* map = maps_ground.start; map != nullptr; map = map->next)
		map->data.Clear();
	maps_ground.clear();

	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(const MapLayer* layer_data)
{
	this->width = layer_data->width;
	this->height = layer_data->height;

	maps.At(layer_data->layer)->data.Clear();
	maps.At(layer_data->layer)->data.Insert(layer_data->tiles, (width + 1) * (height + 1), 0);
}


void j1PathFinding::SetGroundMap(const MapLayer* layer_data)
{
	uint l_width = layer_data->width;
	uint l_height = layer_data->height;
	LayerID l_layer = layer_data->layer;

	maps_ground.At(l_layer)->data.Clear();
	maps_ground.At(l_layer)->data.Insert(new uint[(width + 1) * (height + 1)], (width + 1) * (height + 1), 0);

	uint prev_id = 0, curr_id = 0;

	for (uint x = 0; x < l_width; x++) {
		prev_id = curr_id = 0;
		for (uint y = 0; y < l_height; y++) {
			prev_id = curr_id;
			curr_id = layer_data->tiles[(y * l_width) + x];

			if (prev_id == 0 && curr_id != 0 && CheckBoundaries(iPoint(x, y - 1)))
			{
				memset(&maps_ground.At(l_layer)->data[((y - 1) * l_width) + x], 0, 1);
			}
			else if (prev_id == 0 && curr_id == 0 && CheckBoundaries(iPoint(x + 1, y - 1)) && CheckBoundaries(iPoint(x - 1, y - 1))
				&& (layer_data->tiles[((y) * l_width) + x + 1] != 0 || layer_data->tiles[((y) * l_width) + x - 1] != 0))
			{
				memset(&maps_ground.At(l_layer)->data[((y - 1) * l_width) + x], 0, 1);
			}
			else if (CheckBoundaries(iPoint(x, y - 1)))
				memset(&maps_ground.At(l_layer)->data[((y - 1) * l_width) + x], 1, 1);
		}
	}
}

bool j1PathFinding::Update(float dt)
{
	if (App->debug)
		DebugBlitMap();
	return true;
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos, const LayerID layer, bool ground) const
{
	if (ground)
		return (GetTileAtGround(pos, layer) == 0);
	else
		return true;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos, const LayerID layer) const
{
	if(CheckBoundaries(pos))
		return maps.At((const uint)layer)->data[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAtGround(const iPoint& pos, const LayerID layer) const
{
	if (CheckBoundaries(pos))
		return maps_ground.At(layer)->data[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill, bool ground) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();
	LayerID layer = list_to_fill.layer;

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell, layer, ground))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell, layer, ground))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell, layer, ground))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell, layer, ground))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	if (!ground) {
		// nort-east
		cell.create(pos.x + 1, pos.y + 1);
		if (App->pathfinding->IsWalkable(cell, layer, ground))
			list_to_fill.list.add(PathNode(-1, -1, cell, this));

		// nort-west
		cell.create(pos.x - 1, pos.y + 1);
		if (App->pathfinding->IsWalkable(cell, layer, ground))
			list_to_fill.list.add(PathNode(-1, -1, cell, this));

		// south-east
		cell.create(pos.x + 1, pos.y - 1);
		if (App->pathfinding->IsWalkable(cell, layer, ground))
			list_to_fill.list.add(PathNode(-1, -1, cell, this));

		// south-west
		cell.create(pos.x - 1, pos.y - 1);
		if (App->pathfinding->IsWalkable(cell, layer, ground))
			list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}
	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + ((pos.x != parent->pos.x && pos.y != parent->pos.y) ? 14 : 10);
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination, const LayerID layer, p2DynArray<iPoint>* path, bool ground)
{
	if (!IsWalkable(origin, layer, ground) && !IsWalkable(destination, layer, ground))
		return -1;

	PathList open, closed;
	open.layer = layer;
	closed.layer = layer;
	PathNode node;
	node.pos = origin;
	open.list.add(node);

	last_path = path;

	while (open.list.count() > 0)
	{
		p2List_item<PathNode>* current_item = open.GetNodeLowestScore();
		p2List_item<PathNode>* curr_node = closed.list.add(current_item->data);

		if (curr_node->data.pos == destination)
		{
			last_path->Clear();
			while (curr_node->data.parent != nullptr) {
				last_path->PushBack(curr_node->data.pos);
				curr_node = closed.Find(curr_node->data.parent->pos);
			}
			last_path->Flip();
			return last_path->Count();
		}

		PathList neighbours;
		neighbours.layer = layer;
		curr_node->data.FindWalkableAdjacents(neighbours, ground);

		for (p2List_item<PathNode>* neighbour_node = neighbours.list.start; neighbour_node != nullptr; neighbour_node = neighbour_node->next)
		{
			iPoint neighbour_pos = neighbour_node->data.pos;
			if (closed.Find(neighbour_pos) != nullptr)
				continue;

			if (neighbour_pos.DistanceNoSqrt(origin) > MAX_DISTANCE_ASTAR)
				continue;

			p2List_item<PathNode>* node_open = open.Find(neighbour_pos);
			if (node_open == nullptr)
			{
				neighbour_node->data.CalculateF(destination);
				open.list.add(neighbour_node->data);
			} else {
				int new_g = neighbour_node->data.parent->g + ((neighbour_node->data.pos.x != neighbour_node->data.parent->pos.x && neighbour_node->data.pos.y != neighbour_node->data.parent->pos.y) ? 14 : 10);
				if (new_g < node_open->data.g)
				{
					node_open->data.g = new_g;
					node_open->data.parent = &curr_node->data;
				}
			}
		}
		open.list.del(current_item);
	}
	return -1;
}

void j1PathFinding::DebugBlitMap()
{
	p2List_item<p2DynArray<uint>>* ground_map = maps_ground.At(FRONT_LAYER);
	for (uint x = 0; x < width; x++) {
		for (uint y = 0; y < height; y++) {
			if (ground_map->data[(y * width) + x] != 1)
			{
				iPoint pos(x, y);
				pos = App->map->MapToWorld(pos);
				App->render->DrawQuad(iRect(pos.x, pos.y, 16, 16).toSDL_Rect(), 255, 128, 128, 128, 1.0f);
			}
		}
	}
}