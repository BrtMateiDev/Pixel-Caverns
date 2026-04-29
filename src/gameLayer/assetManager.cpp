#include "assetManager.h"

void AssetManager::loadAll() {
    textures = LoadTexture(RESOURCES_PATH "textures.png");
    selection = LoadTexture(RESOURCES_PATH "selection.png");
    player = LoadTexture(RESOURCES_PATH "player.png");
    tail = LoadTexture(RESOURCES_PATH "tail.png");
    slime = LoadTexture(RESOURCES_PATH "slime.png");
}
