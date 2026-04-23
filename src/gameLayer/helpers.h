#ifndef MYGAME_HELPERS_H
#define MYGAME_HELPERS_H

#pragma once
#include <raylib.h>
#include "physics.h"

inline constexpr float pixel = 1.f / 32.f;

struct AssetManager;

inline Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY) {
    return Rectangle{
        (float) x * cellSizePixelsX, (float) y * cellSizePixelsY, (float) cellSizePixelsX, (float) cellSizePixelsY
    };
}

inline Rectangle getRectangleForEntity(Transform2D transform, float textureW, float textureH) {
    Transform2D result = transform;
    result.w = textureW;
    result.h = textureH;

    result.pos.y -= (result.h - transform.h) / 2;

    return result.getAABB();
}

#endif //MYGAME_HELPERS_H
