#ifndef MYGAME_SLIME_H
#define MYGAME_SLIME_H

#pragma once
#include "physics.h"
#include "assetManager.h"
#include "entityAnimation.h"
#include "entity.h"
#include <raylib.h>
#include <random>

struct AssetManager;

struct Slime : Entity {
    Slime() {
        physics.transform.w = 0.8f;
        physics.transform.h = 0.8f;

        life = getMaxLife();
    }

    EntityAnimation animation;

    Vector2 &getPosition() {
        return physics.transform.pos; //think of this as a shortcut when writing
    }

    void render(AssetManager &assetManager) override; //because of the virtual functions we're inheriting!

    bool update(float dt, EntityUpdateData entityUpdateData) override;

    int getEntityType() { return EntityType_Slime; }

    float getMaxLife() { return 10; }

    enum {
        STATE_WANDERING = 0,
        STATE_CHASING,
    };

    int currentState = STATE_WANDERING;

    float changeStateTimer = 1;
    float jumpTimer = 5;
    float moveSpeed = 0; //this will also be used for direction
};
#endif
