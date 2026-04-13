#include "slime.h"
#include "assetManager.h"
#include "helpers.h"
#include "randomStuff.h"

void Slime::render(AssetManager &assetManager) {
    auto aabb=getRectangleForEntity(physics.transform, 1, 1);

    DrawTexturePro(
        assetManager.slime,
        getTextureAtlas(0, 0, 32, 32),
        aabb,
        {0, 0},
        0.0f,
        WHITE
    );
}

void Slime::update(float dt) {
    //FOR NOW EMPTY
}