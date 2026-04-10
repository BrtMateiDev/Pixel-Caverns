#ifndef MYGAME_GAMEMAP_H
#define MYGAME_GAMEMAP_H

#pragma once
#include <vector>
#include "blocks.h"

struct GameMap {
    int w=0;
    int h=0;
    std::vector<Block> mapData;

    void create(int w, int h);

    Block &getBlockUnsafe(int x, int y);
    Block *getBlockSafe(int x, int y); //safe means it will check if the block is inside the world boundaries
};

#endif //MYGAME_GAMEMAP_H