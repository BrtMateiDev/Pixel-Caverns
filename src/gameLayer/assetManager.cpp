#include "assetManager.h"

void AssetManager::loadAll() {
    textures = LoadTexture(RESOURCES_PATH "TextureSheet.png");
    player = LoadTexture(RESOURCES_PATH "player.png");
    tail = LoadTexture(RESOURCES_PATH "tail.png");
    base = LoadTexture(RESOURCES_PATH "Base.png");
}
