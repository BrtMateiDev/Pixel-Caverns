#ifndef MYGAME_SLIME_H
#define MYGAME_SLIME_H

#pragma once
#include "physics.h"
#include "assetManager.h"
#include <raylib.h>
#include <random>

struct AssetManager;

struct Slime {
    PhysicalEntity physics;

    Slime() {
        physics.transform.w=0.8f;
        physics.transform.h=0.8f;
    }

    Vector2 &getPosition() {
        return physics.transform.pos; //think of this as a shortcut when writing
    }

    void render(AssetManager &assetManager);

    void update(float dt);
};
#endif
