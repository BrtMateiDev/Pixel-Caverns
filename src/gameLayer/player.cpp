#include "player.h"
#include "assetManager.h"
#include "helpers.h"

void Player::render(AssetManager &assetManager) {
    //Pickaxe animation
    if (isSwinging) {
        float swingProgress = swingTimer / maxSwingTime;

        float startAngle = 0.0f;
        float endAngle = 0.0f;

        if (facingDirection == 1) {
            // Looking right
            startAngle = -45.f;
            endAngle = 90.0f;
        } else {
            // Looking left
            startAngle = 45.f;
            endAngle = -90.0f;
        }
        float currentAngle = startAngle + (endAngle - startAngle) * swingProgress;

        float pickWidth = 96 * PIXEL;
        float pickHeight = 110 * PIXEL;

        const auto &pickImage = PickaxeRegistry[currentPickaxe];

        Vector2 pawPos = physics.transform.pos;
        pawPos.x += (10.0f * PIXEL) * facingDirection;
        pawPos.y += (5.0f * PIXEL);

        Vector2 pickPivot = {pickWidth / 2.f, pickHeight}; //bottom center

        Rectangle sourceRec = getTextureAtlas(pickImage.textureIndex, 0, 96, 110);
        sourceRec.width *= facingDirection;
        Rectangle destRec = {pawPos.x, pawPos.y, pickWidth, pickHeight};

        DrawTexturePro(
            assetManager.pickaxe,
            sourceRec,
            destRec,
            pickPivot,
            currentAngle,
            WHITE
        );

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) isSwinging = false;
    }

    //Player rendering
    Vector2 centerPos = physics.transform.pos;

    float visualWidth = SPRITE_W * PIXEL;
    float visualHeight = SPRITE_H * PIXEL;
    Vector2 visualOrigin = {visualWidth / 2.f, visualHeight / 2.f};

    Rectangle sourceRec = {
        0, 0, (float) assetManager.player.width * facingDirection, (float) assetManager.player.height
    };
    Rectangle destRec = {
        centerPos.x - (11 * PIXEL * facingDirection), centerPos.y - (7 * PIXEL), visualWidth, visualHeight
    };

    DrawTexturePro(
        assetManager.player,
        sourceRec,
        destRec,
        visualOrigin,
        0.0f,
        WHITE
    );
}

bool Player::update_pickaxe(float deltaTime, AssetManager &assetManager) {
    if (isSwinging) {
        swingTimer += deltaTime;
        float swingProgress = swingTimer / maxSwingTime;

        if (swingProgress >= 0.5f && !soundPlayedThisSwing) {
            if (physics.transform.pos.y <= 100) {
                SetSoundPitch(assetManager.pickaxeHit, GetRandomValue(70, 130) / 100.0f);
                PlaySound(assetManager.pickaxeHit);
                soundPlayedThisSwing = true;
            } else if (physics.transform.pos.y >= 101 && physics.transform.pos.y <= 149) {
                SetSoundPitch(assetManager.pickaxeHit_echo, GetRandomValue(50, 110) / 100.0f);
                PlaySound(assetManager.pickaxeHit_echo);
                soundPlayedThisSwing = true;
            } else {
                SetSoundPitch(assetManager.pickaxeHit_more_echo, GetRandomValue(50, 90) / 100.0f);
                PlaySound(assetManager.pickaxeHit_more_echo);
                soundPlayedThisSwing = true;
            }
        }

        if (swingTimer >= maxSwingTime) {
            isSwinging = false;
        }
    }
    return true;
}

bool Player::update(float deltaTime, EntityUpdateData entityUpdateData) {
    return true;
}
