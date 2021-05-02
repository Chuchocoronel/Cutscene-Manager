#include "CutsceneManager.h"

#include "Log.h"
#include "Defs.h"
#include "Audio.h"
#include "Render.h"
#include "Entity.h"
#include "Font.h"

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
			else if (string == "text")  newElem->type = TEXT;

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
		ret->destiny.x = step.attribute("x").as_int();
		ret->destiny.y = step.attribute("y").as_int();
	}
	else if (string == "activate") ret->action = ACTIVATE;
	else if (string == "deactivate") ret->action = DEACTIVATE;
	else if (string == "wait")
	{
		ret->action = WAITING;
		ret->duration = step.attribute("time").as_int();
		ret->durationAux = ret->duration;
	}

	SString textString = step.attribute("text").as_string("no");

	if (textString != "no")
	{
		ret->text = textString;
		ret->duration = step.attribute("time").as_int();
		ret->durationAux = ret->duration;
	}

	return ret;
}

Cutscene::Cutscene()
{
	this->active = false;
	this->activeStep = nullptr;
	this->drawableStep = false;
}

Cutscene::~Cutscene()
{}

void Cutscene::LoadEntityElement(Entity* ent, int id)
{
	ListItem<Step*>* it = this->steps.start;
	
	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->entity = ent;

		it = it->next;
	}
}

void Cutscene::LoadFXElement(unsigned int fx, int id)
{
	ListItem<Step*>* it = this->steps.start;

	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->FXId = fx;
		
		it = it->next;
	}
}


void Cutscene::LoadMusicElement(const char* path, int id)
{
	ListItem<Step*>* it = this->steps.start;

	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->musicPath = path;

		it = it->next;
	}
}

void Cutscene::LoadTextElement(Font* font, int id)
{
	ListItem<Step*>* it = this->steps.start;

	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->font = font;

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
		if (this->activeStep->data->element.type == TEXT) this->drawableStep = true;
		else stepFinished = this->activeStep->data->Update(dt);

		if (stepFinished)
		{
			this->activeStep = this->activeStep->next;
			stepFinished = false;
		}
	}
}

void Cutscene::DrawCutscene()
{
	if (this->drawableStep == true)
	{
		bool stepFinished = false;

		stepFinished = this->activeStep->data->Draw();

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
	this->durationAux = -1;
	this->FXId = -1;
	this->element.id = -1;
	this->entity = nullptr;
	this->musicPath = nullptr;
	this->action = StepAction::WAITING;
	this->element.type = StepType::WAIT;
	this->destiny = { 0,0 };
	this->font = nullptr;
	this->text = "";
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
			iPoint movement = { this->destiny.x - this->entity->bounds.x, this->destiny.y - this->entity->bounds.y };
			iPoint distance = { abs(this->destiny.x - this->entity->bounds.x), abs(this->destiny.y - this->entity->bounds.y) };
			iPoint vector;

			if (distance.x != 0) vector.x = movement.x / distance.x;
			else vector.x = 0;
			if (distance.y != 0) vector.y = movement.y / distance.y;
			else vector.y = 0;

			this->entity->bounds.x += vector.x * speed;
			this->entity->bounds.y += vector.y * speed;

			if ((movement.x > -3 && movement.x < 3) && (movement.y > -3 && movement.y < 3)) ret = true;
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
			this->duration -= 1000 * dt;

			if (duration <= 0)
			{
				this->duration = this->durationAux;
				ret = true;
			}
		}
		break;
	default:
		break;
	}

	return ret;
}

bool Step::Draw()
{
	bool ret = false;

	if (this->element.type == TEXT)
	{
		if (this->action == ACTIVATE)
		{
			SDL_Rect r = { 385, 595, 510, 110 };
			app->render->DrawRectangle(r, 0, 0, 0, 255, true);
			SDL_Rect r2 = { 390,600,500,100 };
			app->render->DrawRectangle(r2, 255, 255, 255, 255, true);

			app->render->DrawText(this->font, this->text.GetString(), { 400, 610, 480,80 }, 36, 5, { 0,0,0,255 }, 880);

			this->duration -= 1000 * (1/60);

			if (duration <= 0)
			{
				this->duration = this->durationAux;
				ret = true;
			}
		}
	}

	return ret;
}