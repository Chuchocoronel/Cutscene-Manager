#pragma once

#include "Module.h"
#include "List.h"
#include "Point.h"
#include "Entity.h"

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

public:

	StepAction action;
	Element element;

	Entity* entity;
	iPoint movement;
	int FXId;
	const char* musicPath;
	int duration;
};

class Cutscene
{
public:

	Cutscene();
	~Cutscene();

	void LoadEntityElement(Entity* ent, int id);
	void LoadFXElement(unsigned int fx, int id);
	void LoadMusicElement(const char* path, int id);

	void StartCutscene();
	void UpdateCutscene(float dt);


public:

	List<Step*> steps;
	ListItem<Step*>* activeStep;

	bool active;
};

class CutsceneManager : public Module
{
public:

	CutsceneManager();
	~CutsceneManager();

	Cutscene* LoadCutscene(const char* path);
	Step* LoadStep(pugi::xml_node step);


public:

	bool cutsceneIsActive;
};