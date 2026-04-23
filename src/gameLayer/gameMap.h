#ifndef MYGAME_GAMEMAP_H
#define MYGAME_GAMEMAP_H

#pragma once
#include <cassert>
#include <vector>
#include "blocks.h"

struct GameMap {
    int w = 0;
    int h = 0;

    std::vector<Block> mapData;

    void create(int w, int h) {
        *this = {}; //resets data
        mapData.resize(w * h);

        this->w = w;
        this->h = h;

        for (auto &e: mapData) e = {}; //clears block data
    }

    Block &getBlockUnsafe(int x, int y) {
        assert(mapData.size()==w*h && "The map hasn't been initialized!");
        assert(x>=0 && y>=0 && x<w && y<h && "getBlockUnsafe is out of bounds!");

        return mapData[x + y * w];
    }


    Block *getBlockSafe(int x, int y) {
        assert(mapData.size()==w*h && "The map hasn't been initialized!");
        if (x < 0 || y < 0 || x >= w || y >= h) return nullptr;

        return &mapData[x + y * w];
    } //safe means it will check if the block is inside the world boundaries
};

#endif //MYGAME_GAMEMAP_H
