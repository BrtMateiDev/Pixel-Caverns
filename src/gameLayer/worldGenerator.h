#ifndef MYGAME_WORLDGENERATOR_H
#define MYGAME_WORLDGENERATOR_H

#pragma once
#include <FastNoiseLite.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>
#include "blocks.h"
#include "helpers.h"

struct OreRule {
    unsigned short int blockType;
    uint32_t chance; //10 means 1% chance
};

struct LayerDef {
    int minDepth;
    int maxDepth;
    unsigned short int baseBlock;
    float caveThreshold;
    std::vector<OreRule> ores;
};

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
    static constexpr int SIZE = 32; //1024 blocks per chunk
    Block blocks[SIZE][SIZE];
    bool generated = false;

    Block &getBlockRelative(int x, int y) {
        return blocks[y][x];
    }
};

inline class GenerationOracle {
    FastNoiseLite noise;
    int worldSeed{};

    std::vector<LayerDef> layers;

    const LayerDef *getLayerAt(int y) {
        for (const auto &layer: layers) {
            if (y >= layer.minDepth && y <= layer.maxDepth) return &layer;
        }
        return nullptr;
    }

    unsigned short int calculateOre(int x, int y, const LayerDef &layer) {
        uint32_t hash = calculateHash(x, y, worldSeed);
        uint32_t chance = hash % 1000;

        for (const auto &ore: layer.ores) {
            if (chance < ore.chance) return ore.blockType;
        }
        return layer.baseBlock;
    }

public:
    //Initializing the parameters the noise function will use with just the seed
    void init(int seed) {
        worldSeed = seed;
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetSeed(seed);
        noise.SetFrequency(0.015f);

        // Layer 0: Surface (0 to 20)
        layers.push_back({0, 0, Block::grass, -1.f, {}});
        layers.push_back({1, 20, Block::dirt, -1.f, {}});

        // Layer 1: Stone Layer (21 to 100)
        layers.push_back({
            21, 100, Block::stone, -0.4f, {
                {Block::PLACEHOLDER, 50}, //Ores will go here soon
            }
        });
    }

    unsigned short int getBlockAt(int x, int y) {
        if (y < 0) return Block::air;

        const LayerDef *layer = getLayerAt(y);
        if (!layer) return Block::stone;

        // Cave generation logic
        float noiseValue = noise.GetNoise((float) x, (float) y);
        if (noiseValue <= layer->caveThreshold) {
            return Block::air;
        }

        return calculateOre(x, y, *layer);
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
