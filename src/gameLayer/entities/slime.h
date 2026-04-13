#ifndef MYGAME_SLIME_H
#define MYGAME_SLIME_H

#pragma once
#include "physics.h"
#include "assetManager.h"
#include "entityAnimation.h"
#include <raylib.h>
#include <random>

struct AssetManager;

struct Slime {
    PhysicalEntity physics;

    Slime() {
        physics.transform.w = 0.8f;
        physics.transform.h = 0.8f;
    }

    EntityAnimation animation;

    Vector2 &getPosition() {
        return physics.transform.pos; //think of this as a shortcut when writing
    }

    void render(AssetManager &assetManager);

    void update(float dt, std::ranlux24_base rng, Vector2 playerPosition);

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
