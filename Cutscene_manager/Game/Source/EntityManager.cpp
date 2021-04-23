#include "EntityManager.h"

#include "App.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Start()
{
	return true;
}

bool EntityManager::Update(float dt)
{
	ListItem<Entity*>* item = entities.start;

	while (item != nullptr && item->data->active == true)
	{
		item->data->Update(dt);
		item = item->next;
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	ListItem<Entity*>* item = entities.start;

	while (item != nullptr && item->data->active == true)
	{
		item->data->PostUpdate();
		item = item->next;
	}

	return true;
}

bool EntityManager::CleanUp()
{
	entities.Clear();

	return true;
}

Entity* EntityManager::CreateEntity(iPoint position, EntityType type, bool active)
{
	Entity* ret = new Entity(position, type, active);

	entities.Add(ret);

	return ret;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	int id = entities.Find(entity);

	entities.Del(entities.At(id));
}