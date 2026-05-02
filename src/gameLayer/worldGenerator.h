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

struct SpawnPoint {
    int depth;
    float chance; // 0.0 to 1.0 (e.g. 0.01 for 1%)
};

struct OreRule {
    unsigned short int blockType;
    std::vector<SpawnPoint> points;

    // Calculates the chance at a specific depth using linear interpolation (straight line graph)
    float getChanceAtDepth(int y) const {
        if (points.empty()) return 0.0f;

        // Sort points by depth just in case they aren't
        auto sortedPoints = points;
        std::sort(sortedPoints.begin(), sortedPoints.end(), [](const SpawnPoint &a, const SpawnPoint &b) {
            return a.depth < b.depth;
        });

        if (y <= sortedPoints.front().depth) return sortedPoints.front().chance;
        if (y >= sortedPoints.back().depth) return sortedPoints.back().chance;

        // Find the two points the current depth 'y' falls between
        for (size_t i = 0; i < sortedPoints.size() - 1; ++i) {
            const auto &p1 = sortedPoints[i];
            const auto &p2 = sortedPoints[i + 1];

            if (y >= p1.depth && y <= p2.depth) {
                // Linear interpolation: y = y1 + (x - x1) * (y2 - y1) / (x2 - x1)
                float t = (float) (y - p1.depth) / (p2.depth - p1.depth);
                return p1.chance + t * (p2.chance - p1.chance);
            }
        }
        return 0.0f;
    }
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

    unsigned short int calculateOre(int x, int y, const LayerDef &layer) {
        uint32_t hash = calculateHash(x, y, worldSeed);
        // Use the hash to get a float between 0.0 and 1.0
        float roll = (float) (hash % 1000000) / 1000000.0f;

        float currentThreshold = 0.0f;
        for (const auto &ore: layer.ores) {
            float chance = ore.getChanceAtDepth(y);
            currentThreshold += chance;

            if (roll < currentThreshold) return ore.blockType;
        }
        return layer.baseBlock;
    }

public:
    const LayerDef *getLayerAt(int y) {
        for (const auto &layer: layers) {
            if (y >= layer.minDepth && y <= layer.maxDepth) return &layer;
        }
        return nullptr;
    }

    //Initializing the parameters the noise function will use with just the seed
    void init(int seed) {
        worldSeed = seed;
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetSeed(seed);
        noise.SetFrequency(0.015f);

        // Layer 0: Surface (0 to 5)
        layers.push_back({0, 0, Block::grass, -1.f, {}});
        layers.push_back({1, 5, Block::dirt, -1.f, {}});

        // Layer 1: Stone Layer (6 to 100)
        layers.push_back({
            6, 100, Block::stone_shallow, -0.7f, {
                {
                    Block::coal,
                    {
                        {6, 0.05f}, // 5% at the top of the layer
                        {60, 0.1f}, // Peaks at 10% at depth 60
                        {100, 0.05f} // Fades back to 5% at depth 100
                    }
                },
                {
                    Block::iron,
                    {
                        {20, 0.005f}, // 0.5% starting from depth 20
                        {100, 0.03f}, // Peaks at 3% at the bottom
                    }
                }
            }
        });
    }

    unsigned short int getBlockAt(int x, int y) {
        if (y < 0) return Block::air;

        const LayerDef *layer = getLayerAt(y);
        if (!layer) return Block::PLACEHOLDER;

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

                    if (usesOracle) {
                        newChunk->blocks[dy][dx].type = oracle.getBlockAt(wx, wy);

                        // Cave background
                        if (newChunk->blocks[dy][dx].type == Block::air) {
                            const LayerDef *layer = oracle.getLayerAt(wy);
                            if (layer) {
                                newChunk->blocks[dy][dx].bgTexture = BlockRegistry[layer->baseBlock].bgIndex;
                            } else {
                                newChunk->blocks[dy][dx].bgTexture = 0; // PLACEHOLDER_BG
                            }
                        } else {
                            newChunk->blocks[dy][dx].bgTexture = BlockRegistry[newChunk->blocks[dy][dx].type].bgIndex;
                        }
                    } else {
                        newChunk->blocks[dy][dx].type = Block::air;
                        newChunk->blocks[dy][dx].bgTexture = 0; // PLACEHOLDER_BG
                    }
                }
            }
            mapData[key] = newChunk;
        }
        int dx = ((x % Chunk::SIZE) + Chunk::SIZE) % Chunk::SIZE;
        int dy = ((y % Chunk::SIZE) + Chunk::SIZE) % Chunk::SIZE;

        return mapData[key]->getBlockRelative(dx, dy);
    }
};

static short PH = Block::PLACEHOLDER;
static short PH_BG = Block::PLACEHOLDER_BG;

inline void generatePlayerBase(GameMap &baseMap) {
    const int BASE_WIDTH = 15;
    const int BASE_HEIGHT = 6;

    int playerBase[BASE_HEIGHT][BASE_WIDTH] =
    {
        {PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH},
        {PH, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH},
        {PH, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH},
        {PH, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH},
        {PH, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH_BG, PH},
        {PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH, PH}
    };

    for (int y = 0; y < BASE_HEIGHT; ++y)
        for (int x = 0; x < BASE_WIDTH; ++x)
            baseMap.getBlock(x, y).type = playerBase[y][x];
}

#endif
