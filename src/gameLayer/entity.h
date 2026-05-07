#ifndef MYGAME_ENTITY_H
#define MYGAME_ENTITY_H

#pragma once
#include <random>
#include "physics.h"

struct AssetManager;
struct EntityHolder;

enum EntityType {
    EntityType_Player = 0,
    //EntityType_DroppedItem,
};

struct EntityUpdateData {
    Vector2 playerPosition = {};

    std::ranlux24_base &rng;
    EntityHolder &entityHolder;

    std::uint64_t ownId = 0;
};

/*Pay attention to the struct above. As the code will evolve, new parameters will have to eventually be added, just imagine how
annoying it will be having to manually include the new parameter everywhere. By grouping all of those parameters into this
struct, it will only be needed to add the new parameters right here instead of updating every derived struct.*/

struct Entity {
    PhysicalEntity physics;
    float life = 1;

    Vector2 &getPosition() {
        return physics.transform.pos;
    }

    void teleport(Vector2 pos) {
        physics.teleport(pos);
    }

    //A virtual function is used in polymorphism and is expected to be overridden
    virtual void render(AssetManager &assetManager) = 0;

    virtual bool update(float dt, EntityUpdateData entityUpdateData) = 0;

    virtual int getEntityType() = 0;

    virtual float getMaxLife() = 0; //Placeholder that will be here for a while
};

#endif
