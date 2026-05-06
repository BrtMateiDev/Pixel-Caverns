#include "assetManager.h"

void AssetManager::loadAll() {
    textures = LoadTexture(RESOURCES_PATH "TextureSheet.png");
    player = LoadTexture(RESOURCES_PATH "Player.png");
    base = LoadTexture(RESOURCES_PATH "Base.png");
    pickaxe = LoadTexture(RESOURCES_PATH "stone_pickaxe.png");
    bgSky = LoadTexture(RESOURCES_PATH "background_sky.png");
    bgMountains = LoadTexture(RESOURCES_PATH "background_mountains.png");
}
