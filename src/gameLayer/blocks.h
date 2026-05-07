#ifndef MYGAME_BLOCKS_H
#define MYGAME_BLOCKS_H

#pragma once
#include <cstdint>

namespace Ore {
    enum {
        none = 0,
        coal,
        iron,
        silver,
        gold,
        diamond,

        ORES_COUNT
    };
}

struct BlockProperties {
    const char *name = "Unknown";
    bool isCollidable = true;
    bool isMineable = true;

    float durability = 1.0f;
    short textureIndex = 0; //in the first row
    short oreIndex = 0; //in the second row
    short bgIndex = 0; //in the third row

    int dropType = -1;
    //If not -1, it means the mined block will be different (ex: all stone variants should be stored into "Stone")
};

struct Block {
    enum {
        air,

        dirt,
        grass,

        stone_shallow,

        coal,
        iron,
        silver,

        stone_deepslate,
        gold,
        diamond,

        stone_end,

        INV,
        PLACEHOLDER,
        PLACEHOLDER_BG,

        BLOCKS_COUNT
    };

    std::uint16_t type = 0; //each block will be represented by a 16-bit number
    std::uint16_t bgTexture = 0;

    void sanitize() {
        if (type >= BLOCKS_COUNT) type = 0;
    }

    bool isCollidable();

    bool isMineable();

    float getDurability();
};

extern BlockProperties BlockRegistry[Block::BLOCKS_COUNT];

void initBlockRegistry();

#endif
