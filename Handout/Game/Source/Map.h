#ifndef __MAP_H__
#define __MAP_H__

#include "Render.h"
#include "Textures.h"

#include "List.h"
#include "Point.h"
#include "DynArray.h"

#define COST_MAP_SIZE	100

struct SDL_Texture;

struct TileSet
{
	SString	name;
	int	firstgid;
	int margin;
	int	spacing;
	int	tileWidth;
	int	tileHeight;

	SDL_Texture* texture;
	int	texWidth;
	int	texHeight;
	int	numTilesWidth;
	int	numTilesHeight;
	int	offsetX;
	int	offsetY;

	SDL_Rect GetTileRect(int id) const;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

struct Properties
{
	struct Property
	{
		SString name;
		int value;
	};

	~Properties()
	{
		ListItem<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.Clear();
	}

	int GetProperty(const char* name, int default_value = 0) const;

	List<Property*> list;
};

struct MapLayer
{
	SString	name;
	int width;
	int height;
	uint* data;

	Properties properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y * width) + x];
	}
};

struct MapData
{
	int width;
	int	height;
	int	tileWidth;
	int	tileHeight;
	SDL_Color backgroundColor;
	MapTypes type;
	List<TileSet*> tilesets;

	List<MapLayer*> layers;
};

class Map
{
public:

    Map(Textures* render);

    virtual ~Map();

    bool Awake(pugi::xml_node& conf);

    void Draw(Render* render);

	void DrawLayer(Render* render, int num);

    bool CleanUp();

    bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;

	iPoint WorldToMap(int x, int y) const;

	SDL_Rect GetTilemapRec(int x, int y) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);

	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

	bool drawColliders = false;

private:

	Textures* tex;

    pugi::xml_document mapFile;
    SString folder;
    bool mapLoaded;

	uint32 scale;
	iPoint camOffset;
};

#endif // __MAP_H__