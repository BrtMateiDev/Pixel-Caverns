#include "assetManager.h"

void AssetManager::loadAll() {
    textures = LoadTexture(RESOURCES_PATH "images/TextureSheet.png");
    player = LoadTexture(RESOURCES_PATH "images/Player.png");
    base = LoadTexture(RESOURCES_PATH "images/Base.png");
    pickaxe = LoadTexture(RESOURCES_PATH "images/PickaxeSheet.png");
    bgSky = LoadTexture(RESOURCES_PATH "images/background_sky.png");
    bgMountains = LoadTexture(RESOURCES_PATH "images/background_mountains.png");
    forgeUI = LoadTexture(RESOURCES_PATH "images/ForgeUI.png");
    forgeButtons = LoadTexture(RESOURCES_PATH "images/forgeButtons.png");
    inventoryUI = LoadTexture(RESOURCES_PATH "images/inventoryUI.png");
    storageUI = LoadTexture(RESOURCES_PATH "images/storageUI.png");
    researcherUI = LoadTexture(RESOURCES_PATH "images/Researcher_dialogue.png");

    uiOpen = LoadSound(RESOURCES_PATH "sounds/uiOpen.wav");
    uiClose = LoadSound(RESOURCES_PATH "sounds/uiClose.wav");
    pickaxeHit = LoadSound(RESOURCES_PATH "sounds/pickaxeHit.wav");
    pickaxeHit_echo = LoadSound(RESOURCES_PATH "sounds/pickaxeHit_echo.mp3");
    jump = LoadSound(RESOURCES_PATH "sounds/jump.wav");
    sleep = LoadSound(RESOURCES_PATH "sounds/sleep.wav");
    teleport = LoadSound(RESOURCES_PATH "sounds/teleport.wav");
    craft = LoadSound(RESOURCES_PATH "sounds/craft.wav");
    talkSound = LoadSound(RESOURCES_PATH "sounds/dialogue_sound.wav");

    backgroundMusic = LoadMusicStream(RESOURCES_PATH "sounds/backgroundMusic.mp3");
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
    UnloadTexture(researcherUI);

    UnloadSound(uiOpen);
    UnloadSound(uiClose);
    UnloadSound(pickaxeHit);
    UnloadSound(pickaxeHit_echo);
    UnloadSound(jump);
    UnloadSound(sleep);
    UnloadSound(teleport);
    UnloadSound(craft);
    UnloadSound(talkSound);

    UnloadMusicStream(backgroundMusic);
}
