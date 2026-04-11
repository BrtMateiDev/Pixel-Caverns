#include "worldGenerator.h"
#include "gameMap.h"
#include "randomStuff.h"
#include <raymath.h>
#include <cmath>

void generateWorld(GameMap &gameMap, int seed) {
    const int w=500;
    const int h=500;
    std::ranlux24_base rng(seed);

    gameMap.create(w, h);

    int dirtSize=10;
    int stoneSize=480;

    for (int x=0; x<w; ++x) {
        for (int y=0; y<h; ++y) {
            Block b;
            if (y<h-(dirtSize+stoneSize)){} //air
            else if (y==h-(dirtSize+stoneSize)) b.type=Block::grassBlock;
            else if (y<h-stoneSize) b.type=Block::dirt;
            else {
                b.type=Block::stone;
                if (getRandomChance(rng, 0.01)) b.type=Block::gold;
            }
            gameMap.getBlockUnsafe(x,y)=b;
        }
    }
};

//If you are confused at how the blocks are remembered, notice the "&" in the gameMap parameter
//(which means the changes are saved) and the getBlockUnsafe function using "b".