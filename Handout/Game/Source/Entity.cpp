#include "Entity.h"
#include "App.h"
#include "Render.h"
#include "Input.h"
#include "Defs.h"
#include "CutsceneManager.h"

Entity::Entity(iPoint position, EntityType type, bool active, bool isPlayer)
{
	this->bounds.x = position.x;
	this->bounds.y = position.y;
	this->bounds.w = 25;
	this->bounds.h = 32;

	this->type = type;

	this->active = active;
	this->isPlayer = isPlayer;
}

Entity::~Entity()
{
}

bool Entity::Update(float dt)
{
	if (this->isPlayer == true)
	{
		if (app->cutsceneMan->cutsceneIsActive == false)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) bounds.y -= SPEED * dt;
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) bounds.x -= SPEED * dt;
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) bounds.y += SPEED * dt;
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) bounds.x += SPEED * dt;
		}
	}

	return true;
}

bool Entity::PostUpdate()
{
	if (this->type == EntityType::BLUE) app->render->DrawRectangle(bounds, 0, 0, 255, 255, true);
	else if (this->type == EntityType::RED) app->render->DrawRectangle(bounds, 255, 0, 0, 255, true);

	return true;
}