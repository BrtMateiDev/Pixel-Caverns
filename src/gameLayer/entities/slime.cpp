#include "slime.h"
#include "assetManager.h"
#include "helpers.h"
#include "randomStuff.h"
#include <cassert>

void Slime::render(AssetManager &assetManager) {
    auto aabb = getRectangleForEntity(physics.transform, 1, 1);

    DrawTexturePro(
        assetManager.slime,
        getTextureAtlas(animation.positionX, animation.positionY, 32, 32),
        aabb,
        {0, 0},
        0.0f,
        WHITE
    );
}

void Slime::update(float dt, std::ranlux24_base rng, Vector2 playerPosition) {
    changeStateTimer -= dt;
    if (changeStateTimer < 0) {
        changeStateTimer = getRandomFloat(rng, 1, 6); //think of this as the attention span

        float distanceToPlayer = Vector2Distance(playerPosition, getPosition());
        if (distanceToPlayer < 20) {
            if (getRandomChance(rng, 0.8)) currentState = STATE_CHASING;
            else currentState = STATE_WANDERING;
        } else currentState = STATE_WANDERING;
    }

    if (physics.downTouch) {
        moveSpeed = 0;
        animation.setAnimation(0); //staying
    } else animation.setAnimation(1); //in air
    jumpTimer -= dt;

    switch (currentState) {
        case STATE_WANDERING:
            if (jumpTimer < 0) {
                jumpTimer = getRandomFloat(rng, 2, 5);
                physics.jump(10);
                moveSpeed = getRandomFloat(rng, -4, 4);
            }
            break;

        case STATE_CHASING:
            if (jumpTimer < 0) {
                jumpTimer = getRandomFloat(rng, 1, 2);
                physics.jump(10);
                if (playerPosition.x > getPosition().x)
                    moveSpeed = getRandomFloat(rng, 4, 7);
                else moveSpeed = -getRandomFloat(rng, 4, 7); //notice the minus
            }
            break;

        default: assert(false && "SOMETHING WENT WRONG!");
    }

    if (moveSpeed) getPosition().x += dt * moveSpeed;

    animation.update(dt, 0.08, 7);
}
