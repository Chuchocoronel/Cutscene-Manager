#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "CutsceneManager.h"
#include "Entity.h"
#include "Map.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
	SDL_Texture* img;
	Cutscene* cutscene;
	Entity* entity;
	Map* map;
};

#endif // __SCENE_H__