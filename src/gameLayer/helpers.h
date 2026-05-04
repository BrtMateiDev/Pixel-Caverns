#ifndef MYGAME_HELPERS_H
#define MYGAME_HELPERS_H

#pragma once
#include <raylib.h>
#include <cstdint>

#include "blocks.h"
#include "physics.h"

inline constexpr float BLOCK_SIZE = 64.f;
inline constexpr float TEXTURE_TILE_SIZE = 64.f;
inline constexpr float PIXEL = 1.f / BLOCK_SIZE;

inline Rectangle shrinkUV(Rectangle in, float shrink = 0.1) {
    in.width -= shrink;
    in.height -= shrink;
    in.x += shrink / 2.f;
    in.y += shrink / 2.f;

    return in;
}

//Combining two 32-bit numbers into a 64-bit number
inline uint64_t getCoordinateKey(int x, int y) {
    //using static_cast to take away the sign for negative numbers (NOT equal to positive)
    uint64_t alt_x = static_cast<uint32_t>(x);
    uint64_t alt_y = static_cast<uint32_t>(y);
    return (alt_x << 32) | alt_y;
}

inline Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY) {
    return shrinkUV(Rectangle{
        (float) x * cellSizePixelsX, (float) y * cellSizePixelsY, (float) cellSizePixelsX, (float) cellSizePixelsY
    });
}

inline Rectangle getRectangleForEntity(Transform2D transform, float textureW, float textureH) {
    Transform2D result = transform;
    result.w = textureW;
    result.h = textureH;

    result.pos.y -= (result.h - transform.h) / 2;

    return result.getAABB();
}

#endif //MYGAME_HELPERS_H
