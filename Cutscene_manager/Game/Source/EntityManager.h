#pragma once

#include "Module.h"
#include "Entity.h"

#include "List.h"

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	Entity* CreateEntity(iPoint position, EntityType type, bool active, bool isPlayer);

	void DestroyEntity(Entity* entity);

public:

	List<Entity*> entities;

};