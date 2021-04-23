#pragma once

#include "Point.h"
#include "SString.h"
#include "SDL/include/SDL_rect.h"

enum class EntityType
{
    BLUE,
    RED
};

class Entity
{
public:

    Entity(iPoint position, EntityType type, bool active);

    ~Entity();

    bool Update(float dt);

    bool PostUpdate();

public:

    SDL_Rect bounds;

    EntityType type;

    bool active = true;
};