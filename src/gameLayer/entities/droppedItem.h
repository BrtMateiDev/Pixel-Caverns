#ifndef MYGAME_DROPPEDITEM_H
#define MYGAME_DROPPEDITEM_H

#pragma once
#include <raylib.h>
#include <random>
#include "physics.h"
#include "entity.h"

struct AssetManager;

struct DroppedItem : public Entity {
    DroppedItem() {
        physics.transform.w = 0.8f;
        physics.transform.h = 0.8f;
    }

    int itemType = 0;
    int itemCounter = 1;

    void render(AssetManager &assetManager) override;

    bool update(float dt, EntityUpdateData entityUpdateData) override;

    int getEntityType() { return EntityType_DroppedItem; }

    float getMaxLife() { return 1.f; } //not an actual health bar
};
#endif
