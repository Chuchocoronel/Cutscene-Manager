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

	StepType type;
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

	StepAction action;
	Element element;

	Entity* entity;
	iPoint destiny;
	int FXId;
	const char* musicPath;
	int duration;
	int durationAux;
	Font* font;
	SString text;
};

class Cutscene
{
public:

	Cutscene();
	~Cutscene();

	void LoadEntityElement(Entity* ent, int id);
	void LoadFXElement(unsigned int fx, int id);
	void LoadMusicElement(const char* path, int id);
	void LoadTextElement(Font* font, int id);

	void StartCutscene();
	void UpdateCutscene(float dt);
	void DrawCutscene();


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