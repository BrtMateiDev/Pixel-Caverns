#ifndef MYGAME_HELPERS_H
#define MYGAME_HELPERS_H

#pragma once
#include <raylib.h>
#include "physics.h"

inline constexpr float pixel = 1.f / 32.f;

struct AssetManager;

inline const char *getBlockName(unsigned short int type) {
    static const char *blockNames[] = {
        "Air", "Dirt", "Grass Block", "Stone", "Grass", "Sand", "Sand Ruby", "Sand Stone",
        "Wood Plank", "Stone Bricks", "Clay", "Wood Log", "Leaves", "Copper Ore", "Iron Ore", "Gold Ore",
        "Copper Block", "Iron Block", "Gold Block", "Bricks", "Snow", "Ice", "Ruby Block", "Platform",
        "Workbench", "Glass", "Furnace", "Painting", "Sapling", "Snow Blue Ruby", "Blue Ruby Block",
        "Door", "Jar", "Table", "Wardrobe", "Bookshelf", "Snow Bricks", "Ice Table", "Ice Wardrobe",
        "Ice Bookshelf", "Ice Platform", "Sand Table", "Sand Wardrobe", "Sand Bookshelf", "Sand Platform",
        "Wooden Chest", "Ice Chest", "Sand Chest", "Bone Chest", "Bone Bricks", "Bone Bench",
        "Bone Wardrobe", "Bone Bookshelf", "Bone Platform"
    };

    if (type >= 0 && type < Block::BLOCKS_COUNT) return blockNames[type];
    return "??? (ERROR)";
}

inline Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY) {
    return Rectangle{
        (float) x * cellSizePixelsX, (float) y * cellSizePixelsY, (float) cellSizePixelsX, (float) cellSizePixelsY
    };
}

inline Rectangle getRectangleForEntity(Transform2D transform, float textureW, float textureH) {
    Transform2D result = transform;
    result.w = textureW;
    result.h = textureH;

    result.pos.y -= (result.h - transform.h) / 2;

    return result.getAABB();
}

#endif //MYGAME_HELPERS_H
