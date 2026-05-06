#ifndef MYGAME_ASSETMANAGER_H
#define MYGAME_ASSETMANAGER_H

#pragma once
#include <raylib.h>

struct AssetManager {
    Texture2D textures = {};
    Texture2D player = {};
    Texture2D base = {};
    Texture2D pickaxe = {};
    Texture2D bgSky = {};
    Texture2D bgMountains = {};

    void loadAll();
};

#endif
