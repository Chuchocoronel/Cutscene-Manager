#include "CutsceneManager.h"

#include "App.h"
#include "Log.h"

CutsceneManager::CutsceneManager()
{}

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
		ret = new Cutscene("cutscene1");

		pugi::xml_node node = doc.child("cutscene");

		for (pugi::xml_node it = node.child("data").first_child(); it; it = it.next_sibling())
		{
			Element* newElem = new Element();
			SString string = it.attribute("type").as_string();

			if (string == "entity") newElem->type = ENTITY;
			else if (string == "fx")  newElem->type = FX;
			else if (string == "music")  newElem->type = MUSIC;

			newElem->id = it.attribute("id").as_int();

			ret->elements.Add(newElem);
		}

		for (pugi::xml_node it = node.child("steps").first_child(); it; it = it.next_sibling())
		{
			ret->steps.Add(LoadStep(it));
		}

		ListItem<Step*>* item = ret->steps.start;
		ListItem<Element*>* elemIt = ret->elements.start;

		while (item != nullptr)
		{
			while (elemIt != nullptr)
			{
				if (elemIt->data->id == item->data->element.id)
				{
					item->data->element.type = elemIt->data->type;
					break;
				}

				elemIt = elemIt->next;
			}

			if (item->data->action != WAIT) item->data->duration = -1;
			if (item->data->element.type != FX) item->data->FXId = -1;

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

	if (string == "move_to")
	{
		ret->action = MOVE_TO;
		ret->position.x = step.attribute("x").as_int();
		ret->position.y = step.attribute("y").as_int();
	}
	else if (string == "move")
	{
		ret->action = MOVE;
		ret->position.x = step.attribute("x").as_int();
		ret->position.y = step.attribute("y").as_int();
	}
	else if (string == "activate") ret->action = ACTIVATE;
	else if (string == "deactivate") ret->action = DEACTIVATE;
	else if (string == "wait")
	{
		ret->action = WAIT;
		ret->duration = step.attribute("duration").as_int();
	}

	return ret;
}

Cutscene::Cutscene(SString name)
{
	this->name = name;
}

Cutscene::~Cutscene()
{}

Step::Step()
{}

Step::~Step()
{}