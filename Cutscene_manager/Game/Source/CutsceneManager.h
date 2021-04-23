#pragma once

#include "Module.h"
#include "List.h"
#include "Point.h"

enum StepAction
{
	MOVE_TO,
	MOVE,
	ACTIVATE,
	ACTIVATE_AT,
	DEACTIVATE,
	WAIT
};

enum StepType
{
	ENTITY,
	FX,
	MUSIC
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

public:

	StepAction action;
	Element element;

	iPoint position;
	int FXId;
	float duration;
};

class Cutscene
{
public:

	Cutscene(SString name);
	~Cutscene();

	bool LoadEntityElement(int posX, int posY, int id);
	bool LoadFXElement(unsigned int fx, int id);

	void StartCutscene();

	void MusicStepUpdate(const char* path, int id);

public:

	SString name;
	List<Element*> elements;
	List<Step*> steps;
};

class CutsceneManager : public Module
{
public:

	CutsceneManager();
	~CutsceneManager();

	Cutscene* LoadCutscene(const char* path);
	Step* LoadStep(pugi::xml_node step);


public:

	bool cutsceneActive;
};