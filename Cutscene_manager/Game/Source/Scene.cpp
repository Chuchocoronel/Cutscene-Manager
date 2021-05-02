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

	font = new Font("Assets/Font/font3.xml", app->tex);

	app->entityMan->CreateEntity({ 500,350 }, EntityType::RED, true, false);

	entity = app->entityMan->CreateEntity({ 200,200 }, EntityType::BLUE, true, true);
	cutscene = app->cutsceneMan->LoadCutscene("Assets/Cutscenes/cutscene1.xml");
	cutscene->LoadEntityElement(entity, 1);
	cutscene->LoadFXElement(app->audio->LoadFx("Assets/Audio/Fx/hello_man.wav"), 2);
	cutscene->LoadMusicElement("Assets/Audio/Music/music_spy.ogg", 3);
	cutscene->LoadTextElement(font, 4);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) ret = false;
	if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) cutscene->StartCutscene();
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) map->drawColliders = !map->drawColliders;

	return ret;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (cutscene->active == true) cutscene->UpdateCutscene(dt);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	map->Draw(app->render);

	if (cutscene->active == true) 
		cutscene->DrawCutscene();

	//app->render->DrawText(font, "Prueba de Font", { 500,500, 1000, 100 }, 36, 5, { 255,255,255,255 }, 600);

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	map->CleanUp();

	return true;
}
