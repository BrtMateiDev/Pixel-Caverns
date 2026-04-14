#include "player.h"
#include "assetManager.h"
#include "helpers.h"

void Player::render(AssetManager &assetManager) {
    auto aabb = physics.transform.getAABB();

    DrawTexturePro(
        assetManager.player,
        {0, 0, (float) assetManager.player.width, (float) assetManager.player.height},
        aabb,
        {0, 0},
        0.0f,
        WHITE
    );
}

bool Player::update(float deltaTime, EntityUpdateData entityUpdateData) {
    return true;
}
