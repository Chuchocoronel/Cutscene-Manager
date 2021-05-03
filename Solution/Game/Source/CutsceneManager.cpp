#include "CutsceneManager.h"

#include "Log.h"
#include "Defs.h"
#include "Audio.h"
#include "Render.h"
#include "Entity.h"
#include "Font.h"

// Constructor, initialize all the variables to avoid problems
CutsceneManager::CutsceneManager()
{
	this->cutsceneIsActive = false;
}

CutsceneManager::~CutsceneManager()
{}

// Load Cutscene
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

		// TODO 1: Load the <data/> from the XML file "cutscene1"
		// HINT: You will need a list of elements to save the type and id, to then use the id to match the element with the action
		// Loading data from the elements
		for (pugi::xml_node it = node.child("data").first_child(); it; it = it.next_sibling())
		{
			Element* newElem = new Element();
			SString string = it.attribute("type").as_string();

			// Type of data
			if (string == "entity") newElem->type = ENTITY;
			else if (string == "fx")  newElem->type = FX;
			else if (string == "music")  newElem->type = MUSIC;
			else if (string == "text")  newElem->type = TEXT;

			// Identificator
			newElem->id = it.attribute("id").as_int();

			elements.Add(newElem);
		}

		// Load steps
		for (pugi::xml_node it = node.child("steps").first_child(); it; it = it.next_sibling())
		{
			ret->steps.Add(LoadStep(it));
		}

		ListItem<Step*>* item = ret->steps.start;

		// TODO 3: Discoment the code
		// Put the data in elemens list to steps using the identificator
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

	// Id of the element to use after to complete the initial data of steps
	ret->element.id = step.attribute("id").as_int();
	SString string = step.attribute("action").as_string();

	// TODO 2: Put the data necessary of the step depending on the action
	// HINT: You will find the actions in the ***enum StepActions***, use string above to know the action.
	// HELP: Not necesary to do for the text, but study how its done
	// Load of the corresponding data depending on the type of action
	if (string == "move")
	{
		ret->action = MOVE;
		ret->destiny.x = step.attribute("x").as_int();
		ret->destiny.y = step.attribute("y").as_int();
	}
	else if (string == "activate") ret->action = ACTIVATE;
	else if (string == "activate_at") 
	{
		ret->action = ACTIVATE_AT;
		ret->destiny.x = step.attribute("x").as_int();
		ret->destiny.y = step.attribute("y").as_int();
	}
	else if (string == "deactivate") ret->action = DEACTIVATE;
	else if (string == "wait")
	{
		ret->action = WAITING;
		ret->duration = step.attribute("time").as_int();
		ret->durationAux = ret->duration;
	}

	SString textString = step.attribute("text").as_string("no");

	// special data for the text
	if (textString != "no")
	{
		ret->text = textString;
		ret->duration = step.attribute("time").as_int();
		ret->durationAux = ret->duration;
	}

	return ret;
}

// Constructor, initialize all the variables to avoid problems
Cutscene::Cutscene()
{
	this->active = false;
	this->activeStep = nullptr;
	this->drawableStep = false;
}

Cutscene::~Cutscene()
{}

// Load of the entities in the steps, using the identificator
void Cutscene::LoadEntityElement(Entity* ent, int id)
{
	ListItem<Step*>* it = this->steps.start;
	
	// Putting the corresponding entity pointer in every step this entity takes action
	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->entity = ent;

		it = it->next;
	}
}

// Load of the FX in the steps, using the identificator
void Cutscene::LoadFXElement(unsigned int fx, int id)
{
	ListItem<Step*>* it = this->steps.start;

	// Putting the corresponding unsigned int from the app->audio->LoadFX() in every step this FX takes action
	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->FXId = fx;
		
		it = it->next;
	}
}

// Load of the music in the steps, using the identificator
void Cutscene::LoadMusicElement(const char* path, int id)
{
	ListItem<Step*>* it = this->steps.start;

	// Putting the corresponding path of the music in every step this musics takes action
	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->musicPath = path;

		it = it->next;
	}
}

// Load of the text in the steps, using the identificator
void Cutscene::LoadTextElement(Font* font, int id)
{
	ListItem<Step*>* it = this->steps.start;

	// Putting the corresponding font in every step this text takes action
	while (it != nullptr)
	{
		if (id == it->data->element.id) it->data->font = font;

		it = it->next;
	}
}

// Start the cutscene
void Cutscene::StartCutscene()
{
	// Booleans to use in other parts of the code for diferent things we don't want or we want to happen during the cutscene
	this->active = true;
	app->cutsceneMan->cutsceneIsActive = true;

	// Starting the cutscene
	this->activeStep = this->steps.start;

	ListItem<Step*>* it = this->steps.start;

	// We put all entities in false to avoid problems that can happen with entities that not take part in the cutscene
	// This, probably, depending on the project, it can be erased
	while (it != nullptr)
	{
		if (it->data->element.type == ENTITY)
		{
			it->data->entity->active = false;
		}

		it = it->next;
	}
}

// Here we update the cutscene every time we change step
void Cutscene::UpdateCutscene(float dt)
{
	bool stepFinished = false;

	// We finish the cutscene when there are no more steps
	if (this->activeStep == nullptr)
	{
		this->active = false;
		app->cutsceneMan->cutsceneIsActive = false;
	}
	else
	{
		// We do this because text is necessary to render, in the update it can't be render, this steps is going to be done
		// in the DrawCutscene() function
		if (this->activeStep->data->element.type == TEXT) this->drawableStep = true;
		// We return true if the step is finished, until then, is false
		else stepFinished = this->activeStep->data->Update(dt);

		if (stepFinished)
		{
			// When the step is finished, we change the step to the next
			this->activeStep = this->activeStep->next;
			stepFinished = false;
		}
	}
}

// We draw the steps that need the render to draw
void Cutscene::DrawCutscene()
{
	// If there's no step that needs to draw, don't enter
	if (this->drawableStep == true)
	{
		bool stepFinished = false;

		if (this->activeStep == nullptr)
		{
			this->active = false;
			app->cutsceneMan->cutsceneIsActive = false;
		}
		else
		{
			// Like the update, only changes the function from Update(dt), to Draw() 
			stepFinished = this->activeStep->data->Draw();

			if (stepFinished)
			{
				this->activeStep = this->activeStep->next;
				stepFinished = false;
				this->drawableStep == false;
			}
		}
	}
}

// We clean the lists inside the cutscene
void Cutscene::CleanUp()
{
	this->steps.Clear();
	if (activeStep != nullptr) RELEASE(activeStep);
}

// Constructor, initialize all the variables to avoid problems
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

// We update the step
bool Step::Update(float dt)
{
	bool ret = false;

	int speed = SPEED * dt;

	// Depending on the type of element have specific actions
	// When is finished ret = true
	switch (this->element.type)
	{
	case ENTITY:
		// Entities actions allowed
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
		else if (this->action == ACTIVATE_AT)
		{
			this->entity->active = true;
			this->entity->bounds.x = this->destiny.x;
			this->entity->bounds.y = this->destiny.y;
			ret = true;
		}
		else if (this->action == DEACTIVATE)
		{
			this->entity->active = false;
			ret = true;
		}
		break;
	case FX:
		// FX action allowed
		if (this->action == ACTIVATE)
		{
			app->audio->PlayFx(this->FXId);
			ret = true;
		}
		break;
	case MUSIC:
		// Music actions allowed
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
		// Wait action allowed
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

// We update and draw the steps that need the render to draw
bool Step::Draw()
{
	bool ret = false;

	// When is text, is the only type that needs the render
	// When is finished ret = true
	if (this->element.type == TEXT)
	{
		// Text action allowed
		if (this->action == ACTIVATE)
		{
			SDL_Rect r = { 385, 595, 510, 110 };
			app->render->DrawRectangle(r, 0, 0, 0, 255, true);
			SDL_Rect r2 = { 390,600,500,100 };
			app->render->DrawRectangle(r2, 255, 255, 255, 255, true);

			app->render->DrawText(this->font, this->text.GetString(), { 400, 610, 480,80 }, 24, 5, { 0,0,0,255 }, 880);

			this->duration -= 1000 * 0.01666;

			if (this->duration <= 0)
			{
				this->duration = this->durationAux;
				ret = true;
			}
		}
	}

	return ret;
}