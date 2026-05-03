#include "player.h"
#include "assetManager.h"
#include "helpers.h"


void Player::render_tail(AssetManager &assetManager) {
    Vector2 c;
    if (facingDirection == 1) {
        c = physics.transform.getBottomLeft();
        c.x -= 11 * PIXEL;
    } else {
        c = physics.transform.getBottomRight();
        c.x -= 6 * PIXEL;
    }

    c.y -= 9 * PIXEL;

    DrawTexturePro(
        assetManager.tail, //texture
        {0, 0, (float) assetManager.tail.width * facingDirection, (float) assetManager.tail.height}, //source
        {c.x, c.y, 17 * PIXEL, 9 * PIXEL}, //destination
        {0, 0}, //origin
        0.0f, //rotation
        WHITE //tint
    );
}

void Player::render(AssetManager &assetManager) {
    auto aabb_player = physics.transform.getAABB();

    DrawTexturePro(
        assetManager.player, //texture
        {0, 0, (float) assetManager.player.width * facingDirection, (float) assetManager.player.height}, //source
        aabb_player, //destination
        {0, 0}, //origin
        0.0f, //rotation
        WHITE //tint
    );
}

bool Player::update(float deltaTime, EntityUpdateData entityUpdateData) {
    return true;
}
