#ifndef MYGAME_ASSETMANAGER_H
#define MYGAME_ASSETMANAGER_H

#pragma once
#include <raylib.h>
struct AssetManager {
    Texture2D dirt={};
    Texture2D stone={};

    void loadAll();
};

#endif //MYGAME_ASSETMANAGER_H