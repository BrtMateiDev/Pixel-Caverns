#ifndef MYGAME_ASSETMANAGER_H
#define MYGAME_ASSETMANAGER_H

#pragma once
#include <raylib.h>

struct AssetManager {
    Texture2D textures = {};
    Texture2D player = {};
    Texture2D tail = {};

    void loadAll();
};

#endif //MYGAME_ASSETMANAGER_H
