#include "assetManager.h"

void AssetManager::loadAll() {
    textures = LoadTexture(RESOURCES_PATH "TextureSheet.png");
    player = LoadTexture(RESOURCES_PATH "Player.png");
    base = LoadTexture(RESOURCES_PATH "Base.png");
    pickaxe = LoadTexture(RESOURCES_PATH "PickaxeSheet.png");
    bgSky = LoadTexture(RESOURCES_PATH "background_sky.png");
    bgMountains = LoadTexture(RESOURCES_PATH "background_mountains.png");
    forgeUI = LoadTexture(RESOURCES_PATH "ForgeUI.png");
    forgeButtons = LoadTexture(RESOURCES_PATH "forgeButtons.png");
    inventoryUI = LoadTexture(RESOURCES_PATH "inventoryUI.png");
    storageUI = LoadTexture(RESOURCES_PATH "storageUI.png");

    uiOpen = LoadSound(RESOURCES_PATH "click.wav");
    uiClose = LoadSound(RESOURCES_PATH "click_alt.wav");
    pickaxeHit = LoadSound(RESOURCES_PATH "pickaxeHit.wav");
    pickaxeHit_echo = LoadSound(RESOURCES_PATH "pickaxeHit_echo.wav");
    pickaxeHit_more_echo = LoadSound(RESOURCES_PATH "pickaxeHit_more_echo.wav");
    jump = LoadSound(RESOURCES_PATH "jump.wav");
    sleep = LoadSound(RESOURCES_PATH "sleep.wav");
    teleport = LoadSound(RESOURCES_PATH "teleport.wav");

    backgroundMusic = LoadMusicStream(RESOURCES_PATH "backgroundMusic.mp3");
    backgroundMusic.looping = true;
}

void AssetManager::unloadAll() {
    UnloadTexture(textures);
    UnloadTexture(player);
    UnloadTexture(base);
    UnloadTexture(pickaxe);
    UnloadTexture(bgSky);
    UnloadTexture(bgMountains);
    UnloadTexture(forgeUI);
    UnloadTexture(forgeButtons);
    UnloadTexture(inventoryUI);
    UnloadTexture(storageUI);

    UnloadSound(uiOpen);
    UnloadSound(uiClose);
    UnloadSound(pickaxeHit);
    UnloadSound(pickaxeHit_echo);
    UnloadSound(pickaxeHit_more_echo);
    UnloadSound(jump);
    UnloadSound(sleep);
    UnloadSound(teleport);

    UnloadMusicStream(backgroundMusic);
}
