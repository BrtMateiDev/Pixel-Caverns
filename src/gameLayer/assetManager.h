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
    Texture2D forgeUI = {};
    Texture2D forgeButtons = {};
    Texture2D inventoryUI = {};
    Texture2D storageUI = {};
    Texture2D researcherUI = {};

    Sound uiOpen;
    Sound uiClose;
    Sound pickaxeHit;
    Sound pickaxeHit_echo;
    Sound jump;
    Sound sleep;
    Sound teleport;
    Sound craft;
    Sound talkSound;

    Music backgroundMusic;

    void loadAll();

    //WHY DID I FORGET TO ADD THIS EARLIER, I WAS CAUSING GPU MEMORY LEAKS ALL THIS TIME WITHOUT REALIZING
    void unloadAll();
};

#endif
