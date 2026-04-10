#include "gameMap.h"
#include <cassert>

void GameMap::create(int w, int h) {
    *this={}; //resets data
    mapData.resize(w*h);

    this->w=w;
    this->h=h;

    for (auto &e : mapData) e={}; //clears block data
};

Block &GameMap::getBlockUnsafe(int x, int y) {
    assert(mapData.size()==w*h && "The map hasn't been initialized!");
    assert(x>=0 && y>=0 && x<w && y<h && "getBlockUnsafe is out of bounds!");

    return mapData[x+y*w];
};

Block *GameMap::getBlockSafe(int x, int y) {
    assert(mapData.size()==w*h && "The map hasn't been initialized!");
    if (x<0 || y< 0 || x>=w || y>=h) return nullptr;

    return &mapData[x+y*w];
};
