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

	// TODO 4: Load all elements that participates in the cutscene
	// HINT: You will need 4 entities (2 Reds and 2 blue), 2 FX, 2 Musics and 1 Font
	cutscene = app->cutsceneMan->LoadCutscene("Assets/Cutscenes/cutscene1.xml");
	cutscene->LoadEntityElement(app->entityMan->CreateEntity({ 200,200 }, EntityType::BLUE, true, true), 1);
	cutscene->LoadEntityElement(app->entityMan->CreateEntity({ -39, 336 }, EntityType::BLUE, false, false), 2);
	cutscene->LoadEntityElement(app->entityMan->CreateEntity({ 1057,400 }, EntityType::RED, false, false), 3);
	cutscene->LoadEntityElement(app->entityMan->CreateEntity({ 1003,368 }, EntityType::RED, false, false), 4);
	cutscene->LoadFXElement(app->audio->LoadFx("Assets/Audio/Fx/sword_clash.wav"), 5);
	cutscene->LoadFXElement(app->audio->LoadFx("Assets/Audio/Fx/death.wav"), 6);
	cutscene->LoadMusicElement("Assets/Audio/Music/fight_theme_2.ogg", 7);
	cutscene->LoadMusicElement("Assets/Audio/Music/win.ogg", 8);
	cutscene->LoadTextElement(font, 9);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) ret = false;
	if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) cutscene->StartCutscene();

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

	if (cutscene->active == true) cutscene->DrawCutscene();

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	cutscene->CleanUp();
	RELEASE(cutscene);

	map->CleanUp();

	return true;
}
