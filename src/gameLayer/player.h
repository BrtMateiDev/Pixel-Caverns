#ifndef MYGAME_PLAYER_H
#define MYGAME_PLAYER_H

#pragma once
#include <unordered_map>
#include <raylib.h>

#include "blocks.h"
#include "physics.h"
#include "entity.h"
#include "helpers.h"
#include "pickaxe.h"

struct AssetManager;

struct Player : public Entity {
    static constexpr float SPRITE_W = 112;
    static constexpr float SPRITE_H = 72;

    static constexpr float HITBOX_W = 60.f * PIXEL;
    static constexpr float HITBOX_H = 58.f * PIXEL;

    Player() {
        physics.transform.w = HITBOX_W;
        physics.transform.h = HITBOX_H;

        inventory.minedOres = {};

        life = getMaxLife();
    }

    int currentPickaxe = PickaxeType::Wood;
    bool unlockedPickaxes[PickaxeType::COUNT] = {true, false, false, false, false, false};

    bool isSwinging = false;
    float swingTimer = 0.0f;
    float maxSwingTime = 0.30f;
    bool soundPlayedThisSwing = false;

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

    void startSwing() {
        if (!isSwinging) {
            isSwinging = true;
            swingTimer = 0.0f;
            soundPlayedThisSwing = false;
        }
    }

    void render(AssetManager &assetManager) override;

    bool update(float deltaTime, EntityUpdateData entityUpdateData) override;

    bool update_pickaxe(float deltaTime, AssetManager &assetManager);

    int getEntityType() { return EntityType_Player; }

    float getMaxLife() { return 10; }
};

#endif
