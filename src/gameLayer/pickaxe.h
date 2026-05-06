#ifndef PIXEL_CAVERNS_PICKAXE_H
#define PIXEL_CAVERNS_PICKAXE_H

#pragma once

#include <unordered_map>

namespace PickaxeType {
    enum {
        Wood = 0,
        Stone,
        Iron,
        Silver,
        Gold,
        Diamond,
        God,
        COUNT
    };
}

struct PickaxeProperties {
    const char *name = "Unknown";
    float power = 1.0f;
    float range = 2.5f;
    int textureIndex = 0;
    std::unordered_map<unsigned short int, unsigned int> cost;
};

extern PickaxeProperties PickaxeRegistry[PickaxeType::COUNT];

void initPickaxeRegistry();

#endif
