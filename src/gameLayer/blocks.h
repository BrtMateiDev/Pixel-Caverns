#ifndef MYGAME_BLOCKS_H
#define MYGAME_BLOCKS_H

#pragma once
#include <cstdint>

namespace Ore {
    enum {
        none = 0,
        coal,
        iron,

        ORES_COUNT
    };
}

struct BlockProperties {
    const char *name = "Unknown";
    bool isCollidable = true;
    bool isMineable = true;

    short textureIndex = 0; //in row 1
    short oreIndex = 0; //in row 2
    short bgIndex = 0; //in row 3

    int dropType = -1;
    //If not -1, it means the mined block will be different (ex: all stone variants should be stored into "Stone")

    float durability = 1.0f;
};

struct Block {
    enum {
        air,

        dirt,
        grass,

        stone_shallow,

        coal,
        iron,

        stone_deepslate,

        INV,
        PLACEHOLDER,
        PLACEHOLDER_BG,

        BLOCKS_COUNT
    };

    std::uint16_t type = 0; //each block will be represented by a 16-bit number
    std::uint16_t bgTexture = 0; // Stores the texture index from Row 1

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
