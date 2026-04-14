#ifndef MYGAME_PLAYER_H
#define MYGAME_PLAYER_H

#pragma once
#include <raylib.h>
#include "physics.h"
#include "entity.h"

struct AssetManager;

struct Player : public Entity {
    Player() {
        physics.transform.w = 0.8f;
        physics.transform.h = 1.6f;

        life = getMaxLife();
    }

    Vector2 &getPosition() {
        return physics.transform.pos;
    }

    void render(AssetManager &assetManager) override;

    bool update(float dt, EntityUpdateData entityUpdateData) override;

    int getEntityType() { return EntityType_Player; }

    float getMaxLife() { return 10; }
};

#endif
