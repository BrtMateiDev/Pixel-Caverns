#ifndef MYGAME_ASSETMANAGER_H
#define MYGAME_ASSETMANAGER_H

#pragma once
#include <raylib.h>
struct AssetManager {
    Texture2D dirt={};
    Texture2D textures={};
    Texture2D frame={};

    void loadAll();
};

#endif //MYGAME_ASSETMANAGER_H