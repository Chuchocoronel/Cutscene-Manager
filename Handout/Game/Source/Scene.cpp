#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	map = new Map(app->tex);
	map->Load("map.tmx");

	cutscene = app->cutsceneMan->LoadCutscene("Assets/Cutscenes/cutscene1.xml");

	// TODO 4: Load all elements that participates in the cutscene
	// HINT: You will need 4 entities (2 Reds and 2 blue), 2 FX, 2 Musics and 1 Font

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) ret = false;

	// TODO 5: Start the cutscene
	// EXTRA: It only have to activate once, if activates multiple times, it will crash

	return ret;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	// TODO 5: Update the cutscene
	// EXTRA: Only have to enter when the cutscene is activated

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	map->Draw(app->render);

	// TODO 5: Draw the cutscene
	// EXTRA: Only have to enter when the cutscene is activated

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	// TODO 5: Clean the cutscene

	map->CleanUp();

	return true;
}
