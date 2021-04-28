#include "CutsceneManager.h"

#include "App.h"
#include "Log.h"
#include "Defs.h"
#include "Audio.h"

CutsceneManager::CutsceneManager()
{
	this->cutsceneIsActive = false;
}

CutsceneManager::~CutsceneManager()
{}

Cutscene* CutsceneManager::LoadCutscene(const char* path)
{
	Cutscene* ret = nullptr;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path);

	if (result == NULL) LOG("Could not load the file, pugi error: ", result.description());
	else
	{
		ret = new Cutscene();

		pugi::xml_node node = doc.child("cutscene");

		List<Element*> elements;

		for (pugi::xml_node it = node.child("data").first_child(); it; it = it.next_sibling())
		{
			Element* newElem = new Element();
			SString string = it.attribute("type").as_string();

			if (string == "entity") newElem->type = ENTITY;
			else if (string == "fx")  newElem->type = FX;
			else if (string == "music")  newElem->type = MUSIC;

			newElem->id = it.attribute("id").as_int();

			elements.Add(newElem);
		}

		for (pugi::xml_node it = node.child("steps").first_child(); it; it = it.next_sibling())
		{
			ret->steps.Add(LoadStep(it));
		}

		ListItem<Step*>* item = ret->steps.start;

		while (item != nullptr)
		{
			ListItem<Element*>* elemIt = elements.start;

			while (elemIt != nullptr)
			{
				if (elemIt->data->id == item->data->element.id)
				{
					item->data->element.type = elemIt->data->type;
					break;
				}

				elemIt = elemIt->next;
			}

			item = item->next;
		}
	}
	
	return ret;
}

Step* CutsceneManager::LoadStep(pugi::xml_node step)
{
	Step* ret = new Step();

	ret->element.id = step.attribute("id").as_int();
	SString string = step.attribute("action").as_string();

	if (string == "move")
	{
		ret->action = MOVE;
		ret->movement.x = step.attribute("x").as_int();
		ret->movement.y = step.attribute("y").as_int();
	}
	else if (string == "activate") ret->action = ACTIVATE;
	else if (string == "deactivate") ret->action = DEACTIVATE;
	else if (string == "wait")
	{
		ret->action = WAITING;
		ret->duration = step.attribute("time").as_int() * 100;
	}

	return ret;
}

Cutscene::Cutscene()
{
	this->active = false;
}

Cutscene::~Cutscene()
{}

void Cutscene::LoadEntityElement(Entity* ent, int id)
{
	ListItem<Step*>* it = this->steps.start;
	
	while (it != nullptr)
	{
		if (id == it->data->element.id)
		{
			it->data->entity = ent;
		}

		it = it->next;
	}
}

void Cutscene::LoadFXElement(unsigned int fx, int id)
{
	ListItem<Step*>* it = this->steps.start;

	while (it != nullptr)
	{
		if (id == it->data->element.id)
		{
			it->data->FXId = fx;
		}
		
		it = it->next;
	}
}


void Cutscene::LoadMusicElement(const char* path, int id)
{
	ListItem<Step*>* it = this->steps.start;

	while (it != nullptr)
	{
		if (id == it->data->element.id)
		{
			it->data->musicPath = path;
		}

		it = it->next;
	}
}

void Cutscene::StartCutscene()
{
	this->active = true;
	app->cutsceneMan->cutsceneIsActive = true;

	this->activeStep = this->steps.start;

	ListItem<Step*>* it = this->steps.start;

	while (it != nullptr)
	{
		if (it->data->element.type == ENTITY)
		{
			it->data->entity->active = false;
		}

		it = it->next;
	}
}

void Cutscene::UpdateCutscene(float dt)
{
	bool stepFinished = false;

	if (this->activeStep == nullptr)
	{
		this->active = false;
		app->cutsceneMan->cutsceneIsActive = false;
	}
	else
	{
		stepFinished = this->activeStep->data->Update(dt);

		if (stepFinished)
		{
			this->activeStep = this->activeStep->next;
			stepFinished = false;
		}
	}
}

Step::Step()
{
	this->duration = -1;
	this->FXId = -1;
	this->element.id = -1;
	this->entity = nullptr;
	this->musicPath = nullptr;
	this->action = StepAction::WAITING;
	this->element.type = StepType::WAIT;
	this->movement = { 0,0 };
}

Step::~Step()
{}

bool Step::Update(float dt)
{
	bool ret = false;

	int speed = SPEED * dt;

	switch (this->element.type)
	{
	case ENTITY:
		if (this->action == MOVE)
		{
			this->duration = (abs(this->entity->bounds.x - this->movement.x) + abs(this->entity->bounds.y - this->movement.y)) / speed;

			if (this->entity->bounds.x > this->movement.x) this->entity->bounds.x -= speed;
			else if (this->entity->bounds.x < this->movement.x) this->entity->bounds.x += speed;

			if (this->entity->bounds.y > this->movement.y) this->entity->bounds.y -= speed;
			else if (this->entity->bounds.y < this->movement.y) this->entity->bounds.y += speed;

			if (duration == 0) ret = true;
			if ((this->entity->bounds.y == this->movement.x) && (this->entity->bounds.y == this->movement.y)) ret = true;
		}
		else if (this->action == ACTIVATE)
		{
			this->entity->active = true;
			ret = true;
		}
		else if (this->action == DEACTIVATE)
		{
			this->entity->active = false;
			ret = true;
		}
		break;
	case FX:
		if (this->action == ACTIVATE)
		{
			app->audio->PlayFx(this->FXId);
			ret = true;
		}
		break;
	case MUSIC:
		if (this->action == ACTIVATE)
		{
			app->audio->PlayMusic(this->musicPath, 0);
			ret = true;
		}
		else if (this->action == DEACTIVATE)
		{
			app->audio->PlayMusic("stop", 0);
			ret = true;
		}
		break;
	case WAIT:
		if (this->action == WAITING)
		{
			this->duration -= 60 * dt;

			if (duration <= 0) ret = true;
		}
		break;
	default:
		break;
	}

	return ret;
}