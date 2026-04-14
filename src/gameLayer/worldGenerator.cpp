#include <FastNoiseLite.h>
#include "worldGenerator.h"
#include "gameMap.h"
#include "randomStuff.h"

void generateWorld(GameMap &gameMap, int seed) {
    const int w = 500;
    const int h = 500;
    std::ranlux24_base rng(seed);

    //Check the changelog from the "Added cave (noise) generation" for explanations
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(seed);
    noise.SetFrequency(0.015f);

    gameMap.create(w, h);

    int dirtSize = 10;
    int stoneSize = 480;
    float threshold = -0.5f;

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            Block b;
            if (y < h - (dirtSize + stoneSize)) {
            } //air
            else if (y == h - (dirtSize + stoneSize)) b.type = Block::grassBlock;
            else if (y < h - stoneSize) b.type = Block::dirt;
            else {
                float noiseValue = noise.GetNoise((float) x, (float) y);

                if (noiseValue > threshold) {
                    b.type = Block::stone;
                    if (getRandomChance(rng, 0.01)) b.type = Block::gold;
                }
            }
            gameMap.getBlockUnsafe(x, y) = b;
        }
    }
};

//If you are confused at how the blocks are remembered, notice the "&" in the gameMap parameter
//(which means the changes are saved) and the getBlockUnsafe function using "b".
