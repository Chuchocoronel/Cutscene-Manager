#pragma once

#include "App.h"
#include "List.h"
#include "Point.h"

class Font;
class Entity;

enum StepAction
{
	MOVE,
	ACTIVATE,
	ACTIVATE_AT,
	DEACTIVATE,
	WAITING
};

enum StepType
{
	ENTITY,
	FX,
	MUSIC,
	TEXT,
	WAIT
};

class Element
{
public:

	// What type of element participate
	StepType type;

	// Its identificator
	int id;
};

class Step
{
public:

	Step();
	~Step();

	bool Update(float dt);
	bool Draw();

public:

	// Global data
	StepAction action;
	Element element;

	// For entities
	Entity* entity;
	iPoint destiny;

	// For audio effects
	int FXId;

	// For musics
	const char* musicPath;

	// For texts
	Font* font;
	SString text;

	// For elements that needs a timer
	int duration;
	int durationAux;
};

class Cutscene
{
public:

	Cutscene();
	~Cutscene();

	// To load elements to participate in the cutscene
	void LoadEntityElement(Entity* ent, int id);
	void LoadFXElement(unsigned int fx, int id);
	void LoadMusicElement(const char* path, int id);
	void LoadTextElement(Font* font, int id);

	// To start the cutscene
	void StartCutscene();
	
	// To update the cutscene
	void UpdateCutscene(float dt);

	// To draw the cutscene if necessary
	void DrawCutscene();

	// To clean the cutscene
	void CleanUp();

public:

	List<Step*> steps;
	ListItem<Step*>* activeStep;

	bool drawableStep;
	bool active;
};

class CutsceneManager
{
public:

	CutsceneManager();
	~CutsceneManager();

	Cutscene* LoadCutscene(const char* path);
	Step* LoadStep(pugi::xml_node step);

public:

	bool cutsceneIsActive;
};