#ifndef MYGAME_WORLDGENERATOR_H
#define MYGAME_WORLDGENERATOR_H

#pragma once
#include <FastNoiseLite.h>
#include <unordered_map>
#include <cstdint>
#include "blocks.h"
#include "helpers.h"

//Hash function used for the calculateOre function below
//Also, uint32_t safely handles overflows
inline uint32_t calculateHash(int x, int y, int seed) {
    //These are random prime numbers
    if (!x) x = 873082291;
    if (!seed) seed = 612469133; //(no need for y)

    uint32_t alt_x = x * 379450259;
    uint32_t alt_y = y * 406627657;
    uint32_t alt_seed = seed * 237217483;

    //Using XOR for a straight-forward calculation
    uint32_t hash = alt_x ^ alt_y ^ alt_seed;

    return hash ^ (hash >> 16); //Folding the bits over themselves
}

struct Chunk {
    static constexpr int SIZE = 32; //1024
    Block blocks[SIZE][SIZE];
    bool generated = false;

    Block &getBlockRelative(int x, int y) {
        return blocks[y][x];
    }
};

inline class GenerationOracle {
    FastNoiseLite noise;
    int worldSeed{};

    unsigned short int calculateOre(int x, int y) {
        uint32_t hash = calculateHash(x, y, worldSeed);
        uint32_t chance = hash % 1000;

        //PLACEHOLDERS
        if (chance == 0) return Block::PLACEHOLDER; // 1 in 1000 (0.1%)
        if (chance < 5) return Block::PLACEHOLDER; // 4 in 1000 (0.4%)
        if (chance < 20) return Block::PLACEHOLDER; // 15 in 1000 (1.5%)
        if (chance < 60) return Block::PLACEHOLDER; // 40 in 1000 (4.0%)

        return Block::stone;
    }

public:
    //Initializing the parameters the noise function will use with just the seed
    void init(int seed) {
        worldSeed = seed;
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetSeed(seed);
        noise.SetFrequency(0.015f);
    }

    unsigned short int getBlockAt(int x, int y) {
        //TODO: FINISH DESIGNING THE HARCODED SURFACE
        if (y < 0) return Block::air;
        if (y == 0) return Block::grass;
        if (y < 10) return Block::dirt;

        float noiseValue = noise.GetNoise((float) x, (float) y);
        float threshold = -0.5f; //ARBITRARY

        if (noiseValue <= threshold) {
            return Block::air;
        }

        unsigned short int oreResult = calculateOre(x, y);
        if (oreResult != Block::stone) {
            return oreResult;
        }

        return Block::stone;
    }
} oracle;

struct GameMap {
    int w = 0;
    int h = 0;

    bool usesOracle = false;

    std::unordered_map<uint64_t, Chunk *> mapData;

    //~ means deconstructor
    ~GameMap() {
        for (auto &pair: mapData) {
            delete pair.second;
        }
    }

    Block &getBlock(int x, int y) {
        //Chunk coordinates
        int cx = floor((float) x / Chunk::SIZE);
        int cy = floor((float) y / Chunk::SIZE);
        uint64_t key = getCoordinateKey(cx, cy);

        if (mapData.find(key) == mapData.end()) {
            Chunk *newChunk = new Chunk();

            for (int dy = 0; dy < Chunk::SIZE; ++dy) {
                for (int dx = 0; dx < Chunk::SIZE; ++dx) {
                    //Getting the global world position
                    int wx = cx * Chunk::SIZE + dx;
                    int wy = cy * Chunk::SIZE + dy;

                    if (usesOracle)
                        newChunk->blocks[dy][dx].type = oracle.getBlockAt(wx, wy);
                    else
                        newChunk->blocks[dy][dx].type = Block::air;
                }
            }
            mapData[key] = newChunk;
        }
        int dx = ((x % Chunk::SIZE) + Chunk::SIZE) % Chunk::SIZE;
        int dy = ((y % Chunk::SIZE) + Chunk::SIZE) % Chunk::SIZE;

        return mapData[key]->getBlockRelative(dx, dy);
    }
};

inline void generatePlayerBase(GameMap &baseMap) {
    const int BASE_WIDTH = 15;
    const int BASE_HEIGHT = 6;

    int playerBase[BASE_HEIGHT][BASE_WIDTH] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    for (int y = 0; y < BASE_HEIGHT; ++y)
        for (int x = 0; x < BASE_WIDTH; ++x)
            baseMap.getBlock(x, y).type = playerBase[y][x];
}

#endif
