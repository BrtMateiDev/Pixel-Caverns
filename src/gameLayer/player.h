#ifndef MYGAME_PLAYER_H
#define MYGAME_PLAYER_H

#pragma once
#include <unordered_map>
#include <raylib.h>

#include "blocks.h"
#include "physics.h"
#include "entity.h"
#include "helpers.h"

struct AssetManager;

struct Player : public Entity {
    static constexpr int SPRITE_W = 45;
    static constexpr int SPRITE_H = 36;

    Player() {
        physics.transform.w = SPRITE_W * PIXEL;
        physics.transform.h = SPRITE_H * PIXEL;

        inventory.minedOres = {};

        life = getMaxLife();
    }

    float pickaxePower = 1.0f;

    float speed = 7;

    signed char facingDirection = 1; //since the direction can only be -1 and 1, a signed char is the ideal data type

    struct Inventory {
        std::unordered_map<unsigned short int, unsigned int> minedOres;

        //amount = 1 means the parameter is defaulted as 1 if it's not given
        void mineOre(Block *b, unsigned int amount = 1) {
            int drop = BlockRegistry[b->type].dropType;
            if (drop == -1) drop = b->type;
            minedOres[drop] += amount;
            //Don't worry about uninitialized keys, the compiler will automatically default them to 0
        }
    } inventory;

    Vector2 &getPosition() {
        return physics.transform.pos;
    }

    void render(AssetManager &assetManager) override;

    void render_tail(AssetManager &assetManager);

    bool update(float dt, EntityUpdateData entityUpdateData) override;

    int getEntityType() { return EntityType_Player; }

    float getMaxLife() { return 10; }
};

#endif
