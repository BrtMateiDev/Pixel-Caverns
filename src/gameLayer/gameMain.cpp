#include <raylib.h>
#include <imgui.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "gameMain.h"
#include "assetManager.h"
#include "helpers.h"
#include "worldGenerator.h"
#include "physics.h"
#include "player.h"
#include "pickaxe.h"
#include "storage.h"

static int maxCapacity = 300;

static bool hitbox_toggle = false;
static bool storage_toggle = false;
static bool forge_toggle = false;
static bool music_toggle = true;

static bool canMove = true;

static int selectedForgePickaxe = PickaxeType::Wood;

static bool isSleeping = false;
static float sleepTimer = 0.0f;
static bool worldResetTriggered = false;

static bool isTeleporting = false;
static float teleportTimer = 0.0f;
static bool teleportMapSwitched = false;
static int teleportDestination = 0; // 0 is the mine, 1 is the base

struct GameData {
    GameMap worldMap;
    GameMap baseMap;
    GameMap *activeMap = nullptr;

    Camera2D camera = {};

    Vector2 selectionStart = {};
    Vector2 selectionEnd = {};

    Player player;

    float miningProgress = 0;
    int lastMinedX = -1;
    int lastMinedY = -1;
} gameData;

AssetManager assetManager;

struct OreItem {
    unsigned short type;
    unsigned int amount;
    std::string name;
};

// Sorting the ores alphabetically
std::vector<OreItem> getSortedOres(const std::unordered_map<unsigned short int, unsigned int> &oreMap) {
    std::vector<OreItem> items;
    for (const auto &[type, amount]: oreMap) {
        if (amount > 0) {
            items.push_back({type, amount, BlockRegistry[type].name});
        }
    }

    std::sort(items.begin(), items.end(), [](const OreItem &a, const OreItem &b) {
        return a.name < b.name;
    });

    return items;
}

void execute_tp_mine() {
    gameData.activeMap = &gameData.worldMap;
    gameData.player.teleport({20, 0});
    gameData.player.facingDirection = 1;
    gameData.player.speed = 7;
}

void execute_tp_base() {
    gameData.activeMap = &gameData.baseMap;
    gameData.player.teleport({28, 7});
    gameData.player.facingDirection = -1;
    gameData.player.speed = 5;
}

void tp_mine() {
    if (!isTeleporting && !isSleeping) {
        isTeleporting = true;
        teleportTimer = 0.0f;
        teleportMapSwitched = false;
        teleportDestination = 0;
        canMove = false;
    }
}

void tp_base() {
    if (!isTeleporting && !isSleeping) {
        isTeleporting = true;
        teleportTimer = 0.0f;
        teleportMapSwitched = false;
        teleportDestination = 1;
        canMove = false;
    }
}

void open_forge() {
    float startX = GetScreenWidth() / 2.0f - (384.0f / 2.0f);
    float startY = GetScreenHeight() / 2.0f - (448.0f / 2.0f);

    DrawTextureEx(assetManager.forgeUI, {startX, startY}, 0.0f, 2.0f, WHITE);

    Vector2 mousePos = GetMousePosition();
    const auto &pickProps = PickaxeRegistry[selectedForgePickaxe];

    for (int i = 0; i < PickaxeType::COUNT; i++) {
        int col = (i % 3) * 128;
        int row = (i / 3) * 128;
        Rectangle btnRec = {startX + col, startY + row, 128, 128};

        if (CheckCollisionPointRec(mousePos, btnRec)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectedForgePickaxe = i;
            }
        }

        if (selectedForgePickaxe == i) {
            DrawRectangleLinesEx(btnRec, 4, YELLOW);
        }
    }
    DrawText(TextFormat("%s", pickProps.name), startX + 10, startY + 266, 20, WHITE);
    DrawText(TextFormat("Power: %.1f", pickProps.power), startX + 10, startY + 306, 20, WHITE);
    DrawText(TextFormat("Range: %.1f", pickProps.range), startX + 10, startY + 346, 20, WHITE);

    int yOffset = 266;
    bool canAfford = true;

    for (const auto &[oreType, required]: pickProps.cost) {
        int invAmount = gameData.player.inventory.minedOres[oreType];
        int storageAmount = storage.storedOres[oreType];
        int totalOwned = invAmount + storageAmount;

        if (totalOwned < required) canAfford = false;

        Color textColor = (totalOwned >= required) ? GREEN : RED;
        DrawText(TextFormat("%s: %d / %d", BlockRegistry[oreType].name, totalOwned, required),
                 startX + 202, startY + yOffset, 20, textColor);
        yOffset += 30;
    }

    bool isCrafted = gameData.player.unlockedPickaxes[selectedForgePickaxe];
    bool isEquipped = (gameData.player.currentPickaxe == selectedForgePickaxe);

    int buttonIndex = 0; // "CRAFT PICKAXE!" button
    if (isEquipped) {
        buttonIndex = 3; // "ALREADY EQUIPPED!" button
    } else if (isCrafted) {
        buttonIndex = 2; // "EQUIP PICKAXE!" button
    } else if (!canAfford) {
        buttonIndex = 1; // "NOT ENOUGH ORES!" button
    } else {
        buttonIndex = 0;
    }

    Rectangle craftBtnRec = {startX + 28.0f, startY + 396.0f, 328.0f, 40.0f};
    Rectangle sourceRec = {buttonIndex * 164.0f, 0.0f, 164.0f, 20.0f}; //buttons are 164 pixels in length

    DrawTexturePro(
        assetManager.forgeButtons,
        sourceRec,
        craftBtnRec,
        {0, 0},
        0.0f,
        WHITE
    );

    if ((buttonIndex == 0 || buttonIndex == 2) && CheckCollisionPointRec(mousePos, craftBtnRec)) {
        // Visual feedback experiment
        DrawRectangleRec(craftBtnRec, {255, 255, 255, 50});

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (buttonIndex == 0) {
                for (const auto &[oreType, required]: pickProps.cost) {
                    auto &invAmount = gameData.player.inventory.minedOres[oreType];
                    auto &storageAmount = storage.storedOres[oreType];
                    int amountNeeded = required;

                    if (invAmount >= amountNeeded) {
                        invAmount -= amountNeeded;
                    } else {
                        amountNeeded -= invAmount;
                        invAmount = 0;
                        storageAmount -= amountNeeded;
                    }
                }
                gameData.player.unlockedPickaxes[selectedForgePickaxe] = true;
            }

            gameData.player.currentPickaxe = selectedForgePickaxe;
        }
    }
}

int getTotalOres(const std::unordered_map<unsigned short int, unsigned int> &oreMap) {
    int total = 0;
    for (const auto &[type, amount]: oreMap) {
        total += amount;
    }
    return total;
}

void draw_inventory() {
    float uiScale = 2.0f;
    float destWidth = 128.0f * uiScale;
    float destHeight = 128.0f * uiScale;

    float invX = GetScreenWidth() - destWidth - 10.0f;
    float invY = GetScreenHeight() - destHeight - 10.0f;

    DrawTextureEx(assetManager.inventoryUI, {invX, invY}, 0.0f, uiScale, WHITE);

    auto sortedInventory = getSortedOres(gameData.player.inventory.minedOres);
    int totalOres = getTotalOres(gameData.player.inventory.minedOres);

    float textX = invX + (2.0f * uiScale) + 5.0f;
    float textY = invY + (16.0f * uiScale) + 5.0f;

    for (const auto &item: sortedInventory) {
        DrawText(TextFormat("%s: %u", item.name.c_str(), item.amount), textX, textY, 20, WHITE);
        textY += 24;
    }

    float fillRatio = (float) totalOres / maxCapacity;
    if (fillRatio > 1.0f) fillRatio = 1.0f;

    float barHeight = 20.0f * uiScale;
    float barX = invX + (2.0f * uiScale);
    float barY = invY + destHeight - barHeight - (2.0f * uiScale);
    float barWidth = destWidth - (4.0f * uiScale);

    // Red fill based on the amount of total ores
    DrawRectangle(barX, barY, barWidth * fillRatio, barHeight, RED);
    const char *capText = TextFormat("%d / 300", totalOres);
    int capTextWidth = MeasureText(capText, 20);
    DrawText(capText, barX + (barWidth / 2.0f) - (capTextWidth / 2.0f), barY + 10.0f, 20, WHITE);
}

void open_storage() {
    float uiScale = 2.0f;
    float destWidth = 256.0f * uiScale;
    float destHeight = 256.0f * uiScale;

    float startX = roundf(GetScreenWidth() / 2.0f - (destWidth / 2.0f));
    float startY = roundf(GetScreenHeight() / 2.0f - (destHeight / 2.0f));

    DrawTextureEx(assetManager.storageUI, {startX, startY}, 0.0f, uiScale, WHITE);

    auto sortedStorage = getSortedOres(storage.storedOres);

    float textX = startX + (10.0f * uiScale) + 5.0f;
    float textY = startY + (34.0f * uiScale) + 5.0f;

    for (const auto &item: sortedStorage) {
        DrawText(TextFormat("%s: %u", item.name.c_str(), item.amount), textX, textY, 40, WHITE);
        textY += 50;
    }

    //"DEPOSIT ORES" button
    float btnHeight = 40.0f * uiScale;
    float btnX = startX + (4.0f * uiScale);
    float btnY = startY + destHeight - btnHeight - (4.0f * uiScale);
    float btnWidth = destWidth - (8.0f * uiScale);

    Rectangle btnRec = {btnX, btnY, btnWidth, btnHeight};
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, btnRec)) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            DrawRectangleRec(btnRec, {0, 0, 0, 50});
        } else {
            DrawRectangleRec(btnRec, {255, 255, 255, 50});
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            storage.depositOres(gameData.player.inventory.minedOres);
        }
    }
}

int worldSeed;

void generateSeed(int &worldSeed) {
    std::random_device rd;
    std::mt19937 gen(rd()); //Mersenne Twister, an industry standard random seed generator
    std::uniform_int_distribution<int> dist(-2147483647, 2147483647); //32-bit integer range for the seed
    worldSeed = dist(gen);
}

bool initGame() {
    assetManager.loadAll();

    initBlockRegistry();
    initPickaxeRegistry();
#pragma region sounds
    PlayMusicStream(assetManager.backgroundMusic);

    SetMusicVolume(assetManager.backgroundMusic, 0.2f);
    SetSoundPitch(assetManager.uiOpen, 0.5f);
    SetSoundVolume(assetManager.uiOpen, 0.5f);
    SetSoundPitch(assetManager.uiClose, 0.5f);
    SetSoundVolume(assetManager.uiClose, 0.5f);
    SetSoundPitch(assetManager.jump, 0.5f);
    SetSoundVolume(assetManager.jump, 0.5f);
    SetSoundPitch(assetManager.sleep, 0.5f);
    SetSoundVolume(assetManager.sleep, 0.5f);
    SetSoundPitch(assetManager.teleport, 0.5f);
    SetSoundVolume(assetManager.teleport, 0.5f);
    SetSoundVolume(assetManager.pickaxeHit, 0.3f);
    SetSoundVolume(assetManager.pickaxeHit_echo, 0.4f);
    SetSoundVolume(assetManager.pickaxeHit_more_echo, 0.4f);
#pragma endregion

    generateSeed(worldSeed);
    oracle.init(worldSeed);

    gameData.worldMap.usesOracle = true;
    gameData.baseMap.usesOracle = false;

    generatePlayerBase(gameData.baseMap);
    gameData.activeMap = &gameData.worldMap;

    gameData.camera.target = {20.f, 120.f};
    gameData.camera.rotation = 0.0f;
    gameData.camera.zoom = 200.0f;

    gameData.player.teleport({20, 0});

    return true;
}

bool updateGame() {
    float dt = GetFrameTime();
    //this is delta time which calculates the amount of time between the last and current frames
    if (dt > 1.f / 5) dt = 1.f / 5; //dt is capped at 1/5 in case of lag spikes

    if (music_toggle) UpdateMusicStream(assetManager.backgroundMusic);

    if (gameData.activeMap == &gameData.worldMap) {
        float baseVolume = 0.2f; //20% base volume
        float currentVolume = baseVolume;

        float playerY = gameData.player.physics.transform.pos.y;
        float fadeStartY = 40.0f; //the depth where the music starts fading away
        float fadeEndY = 150.0f; //the depth where the music becomes completely silent

        if (playerY > fadeStartY) {
            float fadeRatio = (playerY - fadeStartY) / (fadeEndY - fadeStartY);
            if (fadeRatio > 1.0f) fadeRatio = 1.0f;

            currentVolume = baseVolume * (1.0f - fadeRatio);
        }

        SetMusicVolume(assetManager.backgroundMusic, currentVolume);
    } else {
        SetMusicVolume(assetManager.backgroundMusic, 0.2f);
    }

    gameData.camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    ClearBackground({75, 75, 150, 255});

    float currentPower = PickaxeRegistry[gameData.player.currentPickaxe].power;
    float currentRange = PickaxeRegistry[gameData.player.currentPickaxe].range;

#pragma region player movement
    if (IsKeyDown(KEY_A) && canMove) {
        gameData.player.physics.transform.pos.x -= gameData.player.speed * dt;
        gameData.player.facingDirection = -1;
    }
    if (IsKeyDown(KEY_D) && canMove) {
        gameData.player.physics.transform.pos.x += gameData.player.speed * dt;
        gameData.player.facingDirection = 1;
    }

    if (IsKeyDown(KEY_SPACE) && gameData.activeMap == &gameData.worldMap) {
        if (gameData.player.physics.jump(10)) {
            SetSoundPitch(assetManager.jump, GetRandomValue(90, 110) / 100.0f);
            PlaySound(assetManager.jump);
        }
    }
#pragma endregion

#pragma region updating physics
    gameData.player.physics.applyGravity();
    gameData.player.physics.updateForces(dt);
    gameData.player.physics.resolveConstraints(*gameData.activeMap);

    gameData.camera.target = gameData.player.physics.transform.pos;

    if (gameData.activeMap == &gameData.baseMap) {
        //clamping X axis so we don't look past the walls
        float screenWorldWidth = GetScreenWidth() / gameData.camera.zoom;
        float minX = screenWorldWidth / 2.0f;
        float maxX = 30.0f - (screenWorldWidth / 2.0f);

        if (maxX < minX) {
            gameData.camera.target.x = 30.0f / 2.0f;
        } else {
            if (gameData.camera.target.x < minX) gameData.camera.target.x = minX;
            if (gameData.camera.target.x > maxX) gameData.camera.target.x = maxX;
        }

        //again clamping, but for Y
        float screenWorldHeight = GetScreenHeight() / gameData.camera.zoom;
        float minY = screenWorldHeight / 2.0f;
        float maxY = 8.0f - (screenWorldHeight / 2.0f);

        if (maxY < minY) {
            gameData.camera.target.y = 8.0f / 2.0f;
        } else {
            if (gameData.camera.target.y < minY) gameData.camera.target.y = minY;
            if (gameData.camera.target.y > maxY) gameData.camera.target.y = maxY;
        }
    }

    gameData.player.physics.updateFinal();
    gameData.player.update_pickaxe(dt, assetManager);

#pragma endregion

#pragma region mouse and keyboard actions
    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);;

    int blockX = floor(worldPos.x);
    int blockY = floor(worldPos.y);

    float distance = Vector2Distance(gameData.player.physics.transform.pos, {blockX + 0.5f, blockY + 0.5f});
    bool inRange = distance <= currentRange;

    if (gameData.activeMap == &gameData.worldMap) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (getTotalOres(gameData.player.inventory.minedOres) < maxCapacity) {
                if (inRange) {
                    auto &b = gameData.worldMap.getBlock(blockX, blockY);
                    if (b.isMineable()) {
                        //Check if we are mining a new block
                        if (blockX != gameData.lastMinedX || blockY != gameData.lastMinedY) {
                            gameData.miningProgress = 0;
                            gameData.lastMinedX = blockX;
                            gameData.lastMinedY = blockY;
                        }

                        gameData.miningProgress += dt * currentPower;

                        if (gameData.miningProgress >= b.getDurability()) {
                            gameData.player.inventory.mineOre(&b);
                            b.type = Block::air;
                            gameData.miningProgress = 0;
                        }
                        gameData.player.startSwing();
                    }
                } else {
                    //Resets the progress if we're trying to mine out of range
                    gameData.miningProgress = 0;
                    gameData.lastMinedX = -1;
                    gameData.lastMinedY = -1;
                }
            } else {
                //Reset the progress if we release the button
                gameData.miningProgress = 0;
                gameData.lastMinedX = -1;
                gameData.lastMinedY = -1;
            }
        }
    }

    if (IsKeyPressed(KEY_SEVEN)) gameData.player.currentPickaxe = PickaxeType::God;
    if (IsKeyPressed(KEY_EIGHT)) maxCapacity = 99999;
    if (IsKeyPressed(KEY_NINE)) maxCapacity = 300;

#pragma endregion

    BeginMode2D(gameData.camera);

#pragma region camera view borders
    Vector2 topLeftView = GetScreenToWorld2D({0, 0}, gameData.camera);
    Vector2 bottomRightView =
            GetScreenToWorld2D({(float) GetScreenWidth(), (float) GetScreenHeight()}, gameData.camera);

    int startXView = (int) floorf(topLeftView.x - 1);
    int endXView = (int) ceilf(bottomRightView.x + 1);
    int startYView = (int) floorf(topLeftView.y - 1);
    int endYView = (int) ceilf(bottomRightView.y + 1);

#pragma endregion

#pragma region drawing the map
    if (gameData.activeMap == &gameData.baseMap) {
        //Drawing the base image as background
        DrawTexturePro(
            assetManager.base,
            {0, 0, (float) assetManager.base.width, (float) assetManager.base.height},
            {0, 0, 30.0f, 10.0f},
            {0, 0},
            0.0f,
            WHITE
        );
    } else {
        if (startYView < 10.0f) {
            float targetSkyHeight = 20.0f;
            float skyScale = targetSkyHeight / assetManager.bgSky.height;
            float targetMountainHeight = 5.0f;
            float mountainScale = targetMountainHeight / assetManager.bgMountains.height;

            float skyY = 0.5f - targetSkyHeight;
            float mountainY = 0.5f - targetMountainHeight;

            //Sky
            float skySpeed = 1.0f;
            float skyWidth = assetManager.bgSky.width * skyScale;
            float skyShiftX = gameData.camera.target.x * skySpeed;
            float skyStartX = floorf((startXView - skyShiftX) / skyWidth) * skyWidth + skyShiftX;

            for (float x = skyStartX; x <= endXView; x += skyWidth) {
                DrawTextureEx(assetManager.bgSky, {x, skyY}, 0.0f, skyScale, WHITE);
            }

            //Mountains
            float mountainSpeed = 0.8f;
            float mountainWidth = assetManager.bgMountains.width * mountainScale;
            float mountainShiftX = gameData.camera.target.x * mountainSpeed;
            float mountainStartX = floorf((startXView - mountainShiftX) / mountainWidth) * mountainWidth +
                                   mountainShiftX;

            for (float x = mountainStartX; x <= endXView; x += mountainWidth) {
                DrawTextureEx(assetManager.bgMountains, {x, mountainY}, 0.0f, mountainScale, WHITE);
            }
        }

        for (int y = startYView; y <= endYView; ++y)
            for (int x = startXView; x <= endXView; ++x) {
                auto &b = gameData.activeMap->getBlock(x, y);
                const auto &props = BlockRegistry[b.type];
                Rectangle destRect = {(float) x, (float) y, 1, 1};

                //Background blocks
                if (b.bgTexture) {
                    DrawTexturePro(
                        assetManager.textures,
                        getTextureAtlas(b.bgTexture, 1, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                        destRect,
                        {0, 0},
                        0.0f,
                        WHITE
                    );
                }

                //Blocks
                if (b.type != Block::air) {
                    DrawTexturePro(
                        assetManager.textures,
                        getTextureAtlas(props.textureIndex + (y >= 101 && props.oreIndex), 0, TEXTURE_TILE_SIZE,
                                        TEXTURE_TILE_SIZE),
                        destRect,
                        {0, 0},
                        0.0f,
                        WHITE
                    );
                }

                //Ores
                if (props.oreIndex != Ore::none) {
                    DrawTexturePro(
                        assetManager.textures,
                        getTextureAtlas(props.oreIndex, 2, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                        destRect,
                        {0, 0},
                        0.0f,
                        WHITE
                    );
                }
            }
    }

#pragma endregion

#pragma region visualizing block selection and mining cracks
    if (gameData.activeMap == &gameData.worldMap) {
        //Main selection highlight
        auto &b = gameData.worldMap.getBlock(blockX, blockY);
        if (b.isMineable()) {
            DrawTexturePro(
                assetManager.textures,
                getTextureAtlas(0, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                {(float) blockX, (float) blockY, 1, 1},
                {0, 0},
                0.f,
                WHITE
            );
        }

        //Mining cracks animation
        if (gameData.lastMinedX == blockX && gameData.lastMinedY == blockY && gameData.miningProgress > 0) {
            float durability = b.getDurability();
            if (durability > 0) {
                int frame = (int) ((gameData.miningProgress / durability) * 4); //4 frames
                if (frame > 3) frame = 3;

                DrawTexturePro(
                    assetManager.textures,
                    getTextureAtlas(frame, 4, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                    {(float) blockX, (float) blockY, 1, 1},
                    {0, 0},
                    0.f,
                    WHITE
                );
            }
        }
    }
#pragma endregion

#pragma region drawing the player
    gameData.player.render(assetManager);

    //Hitbox visualization
    if (hitbox_toggle) DrawRectangleLinesEx(gameData.player.physics.transform.getAABB(), 0.05, {20, 101, 250, 120});
#pragma endregion

    EndMode2D();

#pragma region Shaders (somewhat)
    if (gameData.activeMap == &gameData.worldMap && !isSleeping && !isTeleporting) {
        float playerY = gameData.player.physics.transform.pos.y;
        float startDarkY = 20.0f;
        float maxDarkY = 150.0f;

        if (playerY > startDarkY) {
            float darknessAlpha = (playerY - startDarkY) / (maxDarkY - startDarkY);
            if (darknessAlpha > 0.92f) darknessAlpha = 0.92f;

            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, darknessAlpha));

            BeginBlendMode(BLEND_ADDITIVE); //raylib's blending mode

            Vector2 topLeft = GetScreenToWorld2D({0, 0}, gameData.camera);
            Vector2 bottomRight = GetScreenToWorld2D({(float) GetScreenWidth(), (float) GetScreenHeight()},
                                                     gameData.camera);

            int startX = (int) floorf(topLeft.x - 1);
            int endX = (int) ceilf(bottomRight.x + 1);
            int startY = (int) floorf(topLeft.y - 1);
            int endY = (int) ceilf(bottomRight.y + 1);

            for (int y = startY; y <= endY; ++y) {
                for (int x = startX; x <= endX; ++x) {
                    auto &b = gameData.worldMap.getBlock(x, y);

                    if (b.type == Block::gold || b.type == Block::diamond) {
                        Vector2 screenPos = GetWorldToScreen2D({x + 0.5f, y + 0.5f}, gameData.camera);
                        float oreGlowRadius = 500.0f;

                        if (b.type == Block::gold) {
                            Color goldColor = {255, 244, 134, 80};
                            DrawCircleGradient((int) screenPos.x, (int) screenPos.y, oreGlowRadius, goldColor, BLANK);
                        } else if (b.type == Block::diamond) {
                            Color diamondColor = {151, 223, 250, 80};
                            DrawCircleGradient((int) screenPos.x, (int) screenPos.y, oreGlowRadius, diamondColor,
                                               BLANK);
                        }
                    }
                }
            }
            //The light starts fading in at y 40, and reaches max power at y 125
            float lightStartY = 40.0f;
            float lightMaxY = 125.0f;
            float lightIntensity = 0.0f;

            if (playerY > lightStartY) {
                lightIntensity = (playerY - lightStartY) / (lightMaxY - lightStartY);
                if (lightIntensity > 0.8f) lightIntensity = 0.8f;
            }

            unsigned char finalAlpha = (unsigned char) (150.0f * lightIntensity);

            if (finalAlpha > 0) {
                Vector2 screenPlayerPos = GetWorldToScreen2D(gameData.player.physics.transform.pos, gameData.camera);

                Color lightColor = {255, 210, 210, finalAlpha};
                float lightRadius = 400.0f;

                DrawCircleGradient((int) screenPlayerPos.x, (int) screenPlayerPos.y, lightRadius, lightColor, BLANK);
            }

            EndBlendMode();
        }
    }
#pragma endregion

#pragma region HUD
    if (gameData.activeMap == &gameData.worldMap) {
        float buttonSize = 120; //small offset (instead of 128) because the button has round corners
        int buttonIndex_tp = 1;
        int buttonIndex_sound = 1;

        Rectangle tpButton = {
            4,
            (float) GetScreenHeight() - buttonSize - 4,
            buttonSize,
            buttonSize,
        };

        Rectangle soundButton = {
            132,
            (float) GetScreenHeight() - buttonSize - 4,
            buttonSize,
            buttonSize,
        };
        Vector2 mousePos = GetMousePosition();
        bool isHovered_tp = CheckCollisionPointRec(mousePos, tpButton);
        bool isHeldDown_tp = isHovered_tp && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool isReleased_tp = isHovered_tp && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        bool isHovered_sound = CheckCollisionPointRec(mousePos, soundButton);
        bool isHeldDown_sound = isHovered_sound && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool isReleased_sound = isHovered_sound && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        if (!isHovered_tp) buttonIndex_tp = 1;
        if (isHovered_tp) buttonIndex_tp = 2;
        if (isHeldDown_tp) {
            buttonIndex_tp = 3;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) PlaySound(assetManager.uiOpen);
        }
        if (isReleased_tp) {
            tp_base();
            PlaySound(assetManager.teleport);
        }

        if (!isHovered_sound) buttonIndex_sound = 4;
        if (isHovered_sound) buttonIndex_sound = 5;
        if (isHeldDown_sound) {
            buttonIndex_sound = 6;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) PlaySound(assetManager.uiOpen);
        }
        if (isReleased_sound) {
            music_toggle = !music_toggle;
            PlaySound(assetManager.uiClose);
        }

        DrawTexturePro(
            assetManager.textures,
            getTextureAtlas(buttonIndex_tp, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
            {0, (float) GetScreenHeight() - 128, 128, 128},
            {0, 0},
            0.f,
            WHITE
        );

        DrawTexturePro(
            assetManager.textures,
            getTextureAtlas(buttonIndex_sound, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
            {128, (float) GetScreenHeight() - 128, 128, 128},
            {0, 0},
            0.f,
            WHITE
        );

        //Red X overlay over the music button
        if (!music_toggle) {
            DrawTexturePro(
                assetManager.textures,
                getTextureAtlas(7, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                {128, (float) GetScreenHeight() - 128, 128, 128},
                {0, 0},
                0.f,
                WHITE
            );
        }

        int playerX = (int) floorf(gameData.player.physics.transform.pos.x);
        int playerY = (int) floorf(gameData.player.physics.transform.pos.y);
        const char *coordText = TextFormat("(X: %d; Y: %d)", playerX, playerY);
        int coordTextWidth = MeasureText(coordText, 40);
        DrawText(coordText, GetScreenWidth() / 2 - coordTextWidth / 2, 15, 40, RAYWHITE);

        if (playerY >= 6 && playerY <= 100) {
            const char *layerText = "Shallow";
            int layerWidth = MeasureText(layerText, 40);
            DrawText(layerText, GetScreenWidth() / 2 - layerWidth / 2, 55, 40, RAYWHITE);
        } else if (playerY >= 101) {
            const char *layerText = "Deepslate";
            int layerWidth = MeasureText(layerText, 40);
            DrawText(layerText, GetScreenWidth() / 2 - layerWidth / 2, 55, 40, RAYWHITE);
        }
        if (playerY >= 200) {
            const char *line1 = "Congratulations! You have reached the end of this demo.";
            const char *line2 = "I hope you had fun playing this project :)";

            int width1 = MeasureText(line1, 40);
            int width2 = MeasureText(line2, 40);

            int screenCenter = GetScreenWidth() / 2;
            int startY = (GetScreenHeight() / 2) - 400;

            DrawText(line1, screenCenter - (width1 / 2), startY, 40, YELLOW);
            DrawText(line2, screenCenter - (width2 / 2), startY + 50, 40, YELLOW);
        }
    }

    draw_inventory();

    if (gameData.activeMap == &gameData.baseMap) {
        float px = gameData.player.physics.transform.pos.x;

        const char *prompt = nullptr;
        if (px >= 5 && px <= 6) {
            prompt = "Press [E] to talk to the researcher (NOT IMPLEMENTED)";

            if (IsKeyPressed(KEY_E)) {
                //TODO: ADD INTERACTION
            }
        } else if (px >= 8 && px <= 13) {
            if (!storage_toggle)
                prompt = "Press [E] to open the storage";
            else
                prompt = "Press [E] to close";

            if (IsKeyPressed(KEY_E)) {
                storage_toggle = !storage_toggle;
                canMove = !canMove;

                if (storage_toggle) PlaySound(assetManager.uiOpen);
                else PlaySound(assetManager.uiClose);
            }
        } else if (px >= 17 && px <= 22) {
            if (!forge_toggle)
                prompt = "Press [E] to use the pickaxe forge";
            else
                prompt = "Press [E] to close";

            if (IsKeyPressed(KEY_E)) {
                forge_toggle = !forge_toggle;
                canMove = !canMove;

                if (forge_toggle) PlaySound(assetManager.uiOpen);
                else PlaySound(assetManager.uiClose);
            }
        } else if (px >= 25 && px <= 27) {
            if (!isSleeping) prompt = "Press [E] to sleep (reset mine)";
            if (IsKeyPressed(KEY_E)) {
                isSleeping = true;
                sleepTimer = 0.0f;
                worldResetTriggered = false;
                canMove = false;
                PlaySound(assetManager.sleep);
            }
        } else if (px >= 28 && px <= 29) {
            prompt = "Press [E] to exit the base";

            if (IsKeyPressed(KEY_E)) {
                tp_mine();
                PlaySound(assetManager.teleport);
            }
        }

        if (storage_toggle) open_storage();
        if (forge_toggle) open_forge();

        if (prompt != nullptr && !isSleeping) {
            Vector2 screenPos = GetWorldToScreen2D({px, 4.0f}, gameData.camera);

            int textWidth = MeasureText(prompt, 40);
            DrawRectangle((int) screenPos.x - textWidth / 2 - 5, (int) screenPos.y - 195, textWidth + 10, 50,
                          {0, 0, 0, 150});
            DrawText(prompt, (int) screenPos.x - textWidth / 2, (int) screenPos.y - 190, 40, YELLOW);
        }
    }

    if (isSleeping) {
        sleepTimer += dt;

        if (sleepTimer >= 1.0f && !worldResetTriggered) {
            worldResetTriggered = true;

            generateSeed(worldSeed);
            oracle.init(worldSeed);
            gameData.worldMap.mapData.clear();
        }

        if (sleepTimer >= 2.5f) {
            isSleeping = false;
            canMove = true;
        }
    }

    if (isTeleporting) {
        teleportTimer += dt;

        if (teleportTimer >= 0.5f && !teleportMapSwitched) {
            teleportMapSwitched = true;
            if (teleportDestination == 0) {
                execute_tp_mine();
            } else {
                execute_tp_base();
            }
        }

        if (teleportTimer >= 1.0f) {
            isTeleporting = false;
            canMove = true;
        }
    }

    if (isSleeping) {
        float alpha = 0.0f;
        if (sleepTimer <= 0.5f) alpha = sleepTimer / 0.5f;
        else if (sleepTimer > 0.5f && sleepTimer <= 2.0f) alpha = 1.0f;
        else alpha = 1.0f - ((sleepTimer - 2.0f) / 0.5f);

        if (alpha < 0.0f) alpha = 0.0f;
        if (alpha > 1.0f) alpha = 1.0f;

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));

        if (alpha == 1.0f) {
            int textWidth = MeasureText("Zzz...", 40);
            DrawText("Zzz...", GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2, 40, WHITE);
        }
    }

    if (isTeleporting) {
        float alpha = 0.0f;
        if (teleportTimer <= 0.3f) alpha = teleportTimer / 0.3f;
        else if (teleportTimer > 0.3f && teleportTimer <= 0.7f) alpha = 1.0f;
        else alpha = 1.0f - ((teleportTimer - 0.7f) / 0.3f);

        if (alpha < 0.0f) alpha = 0.0f;
        if (alpha > 1.0f) alpha = 1.0f;

        Color teleportColor = {10, 80, 200, 255};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(teleportColor, alpha));

        if (alpha == 1.0f) {
            const char *tpText = (teleportDestination == 0) ? "Entering Mine" : "Returning to Base";
            int textWidth = MeasureText(tpText, 40);
            DrawText(tpText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2, 40, WHITE);
        }
    }
#pragma endregion

#pragma region imgui_windows

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);

    ImGui::Begin("Dev tools");

    ImGui::SliderFloat("Camera zoom", &gameData.camera.zoom, 5, 250);
    ImGui::SliderFloat("Player speed", &gameData.player.speed, 1, 100);
    ImGui::Checkbox("Show player hitbox", &hitbox_toggle);

    ImGui::Text("FPS: %d", GetFPS());

    ImGui::End();

#pragma endregion

    return true;
}

void closeGame() {
    assetManager.unloadAll();
}
