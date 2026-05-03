#include <raylib.h>
#include <imgui.h>
#include "gameMain.h"
#include "assetManager.h"
#include "helpers.h"
#include "worldGenerator.h"
#include "physics.h"
#include "player.h"
#include "playerBase/storage.h"

static bool toggle = false;

struct GameData {
    GameMap worldMap;
    GameMap baseMap;
    GameMap *activeMap = nullptr;

    Camera2D camera = {};

    int creativeSelectedBlock = Block::stone_shallow;

    Vector2 selectionStart = {};
    Vector2 selectionEnd = {};

    Player player;

    float miningProgress = 0;
    int lastMinedX = -1;
    int lastMinedY = -1;

    //EntityHolder entities;
    //EntityHolder *activeEntities = nullptr;
} gameData;

AssetManager assetManager;

bool initGame() {
    initBlockRegistry();
    assetManager.loadAll();

    oracle.init(1);
    gameData.worldMap.usesOracle = true;
    gameData.baseMap.usesOracle = false; //Turning off the generation for the base

    generatePlayerBase(gameData.baseMap);

    gameData.activeMap = &gameData.worldMap;

    gameData.camera.target = {20.f, 120.f};
    gameData.camera.rotation = 0.0f;
    gameData.camera.zoom = 250.0f;

    gameData.player.teleport({20, 0});
    gameData.player.physics.transform.w = assetManager.player.width * PIXEL;
    gameData.player.physics.transform.h = assetManager.player.height * PIXEL;

    return true;
}

bool updateGame() {
    float dt = GetFrameTime();
    //this is delta time which calculates the amount of time between the last and current frames
    if (dt > 1.f / 5) dt = 1.f / 5; //dt is capped at 1/5 in case of lag spikes

    gameData.camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    ClearBackground({75, 75, 150, 255});

#pragma region player movement
    static float PLAYER_SPEED = 10;
    if (IsKeyDown(KEY_A)) {
        gameData.player.physics.transform.pos.x -= PLAYER_SPEED * dt;
        gameData.player.facingDirection = -1;
    }
    if (IsKeyDown(KEY_D)) {
        gameData.player.physics.transform.pos.x += PLAYER_SPEED * dt;
        gameData.player.facingDirection = 1;
    }

    if (IsKeyDown(KEY_SPACE)) gameData.player.physics.jump(10);
#pragma endregion

#pragma region updating physics
    //player
    gameData.player.physics.applyGravity();
    gameData.player.physics.updateForces(dt);
    gameData.player.physics.resolveConstraints(*gameData.activeMap);
    gameData.camera.target = gameData.player.physics.transform.pos;
    gameData.player.physics.updateFinal();

#pragma endregion

#pragma region mouse and keyboard actions
    if (IsKeyPressed(KEY_MINUS)) {
        gameData.creativeSelectedBlock--;
        if (gameData.creativeSelectedBlock <= 0) gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1;
    }
    if (IsKeyPressed(KEY_EQUAL)) {
        gameData.creativeSelectedBlock++;
        if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT - 2) gameData.creativeSelectedBlock = 1;
    }

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);;

    int blockX = floor(worldPos.x);
    int blockY = floor(worldPos.y);

    float distance = Vector2Distance(gameData.player.physics.transform.pos, {blockX + 0.5f, blockY + 0.5f});
    bool inRange = distance <= 2.5f;
    // A distance of 2.5 covers exactly the block you are in and the immediately adjacent ones

    if (gameData.activeMap == &gameData.worldMap) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (inRange) {
                auto &b = gameData.worldMap.getBlock(blockX, blockY);
                if (b.isMineable()) {
                    // Check if we are mining a new block
                    if (blockX != gameData.lastMinedX || blockY != gameData.lastMinedY) {
                        gameData.miningProgress = 0;
                        gameData.lastMinedX = blockX;
                        gameData.lastMinedY = blockY;
                    }

                    gameData.miningProgress += dt * gameData.player.pickaxePower;

                    if (gameData.miningProgress >= b.getDurability()) {
                        gameData.player.inventory.mineOre(&b);
                        b.type = Block::air;
                        gameData.miningProgress = 0;
                    }
                }
            } else {
                // Reset progress if trying to mine out of range
                gameData.miningProgress = 0;
                gameData.lastMinedX = -1;
                gameData.lastMinedY = -1;
            }
        } else {
            // Reset progress if we release the button
            gameData.miningProgress = 0;
            gameData.lastMinedX = -1;
            gameData.lastMinedY = -1;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && inRange) {
            auto &b = gameData.worldMap.getBlock(blockX, blockY);
            b.type = gameData.creativeSelectedBlock;
        }
    }

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
    for (int y = startYView; y <= endYView; ++y)
        for (int x = startXView; x <= endXView; ++x) {
            auto &b = gameData.activeMap->getBlock(x, y);
            const auto &props = BlockRegistry[b.type];
            Rectangle destRect = {(float) x, (float) y, 1, 1};

            // 1. Draw background
            DrawTexturePro(
                assetManager.textures,
                getTextureAtlas(b.bgTexture, 1, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                destRect,
                {0, 0},
                0.0f,
                WHITE
            );

            // 2. Draw foreground
            if (b.type != Block::air) {
                DrawTexturePro(
                    assetManager.textures,
                    getTextureAtlas(props.textureIndex, 0, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                    destRect,
                    {0, 0},
                    0.0f,
                    WHITE
                );
            }

            // 3. Draw ore
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
#pragma endregion

#pragma region drawing entities (DISABLED)
    // if (gameData.activeEntities)
    //     for (auto &e: gameData.entities.entities)
    //         e.second->render(assetManager);

#pragma endregion

#pragma region drawing the player
    gameData.player.render_tail(assetManager);
    gameData.player.render(assetManager);

    //Hitbox visualization
    if (toggle) DrawRectangleLinesEx(gameData.player.physics.transform.getAABB(), 0.05, {20, 101, 250, 120});
#pragma endregion

#pragma region visualizing block selection and mining cracks
    if (gameData.activeMap == &gameData.worldMap && inRange) {
        // Draw the main selection highlight
        DrawTexturePro(
            assetManager.textures,
            getTextureAtlas(1, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
            {(float) blockX, (float) blockY, 1, 1},
            {0, 0},
            0.f,
            WHITE
        );

        // Draw mining cracks if we are currently mining this block
        if (gameData.lastMinedX == blockX && gameData.lastMinedY == blockY && gameData.miningProgress > 0) {
            auto &b = gameData.worldMap.getBlock(blockX, blockY);
            float durability = b.getDurability();
            if (durability > 0) {
                int frame = (int) ((gameData.miningProgress / durability) * 4); // 4 frames
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

    EndMode2D();

#pragma region HUD
    // Coordinate System (X: [...]; Y: [...])
    int playerX = (int) floorf(gameData.player.physics.transform.pos.x);
    int playerY = (int) floorf(gameData.player.physics.transform.pos.y);
    const char *coordText = TextFormat("(X: %d; Y: %d)", playerX, playerY);
    int textWidth = MeasureText(coordText, 40);
    DrawText(coordText, GetScreenWidth() / 2 - textWidth / 2, 15, 40, RAYWHITE);
#pragma endregion

#pragma region imgui_windows

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);

    ImGui::Begin("Dev tools");

    ImGui::SliderFloat("Camera zoom", &gameData.camera.zoom, 5, 250);
    ImGui::SliderFloat("Player speed", &PLAYER_SPEED, 5, 100);
    ImGui::SliderFloat("Pickaxe power", &gameData.player.pickaxePower, 1, 10);
    ImGui::Checkbox("Show player hitbox", &toggle);

    if (ImGui::Button("Teleport to base")) {
        gameData.activeMap = &gameData.baseMap;
        gameData.player.teleport({5, 2});
        //gameData.activeEntities = nullptr;
    }
    if (ImGui::Button("Teleport to mine")) {
        gameData.activeMap = &gameData.worldMap;
        gameData.player.teleport({20, 0});
        //gameData.activeEntities = &gameData.entities;
    }

    if (gameData.activeMap == &gameData.baseMap) {
        if (ImGui::Button("Deposit ores")) storage.depositOres(gameData.player.inventory.minedOres);
    }

    ImGui::Text("FPS: %d", GetFPS());

    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(
            "Press F11 for fullscreen and ESC to close the game\n"
            "You can also press -/+ to scroll through available blocks");
        ImGui::EndTooltip();
    }
    ImGui::End();

    //Player inventory window
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 250, GetScreenHeight() - 300), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Always);

    ImGui::Begin("Inventory");

    for (const auto &[type, amount]: gameData.player.inventory.minedOres) {
        ImGui::Text("%s: %d", BlockRegistry[type].name, amount);
    }
    ImGui::End();

    //Base storage window
    if (gameData.activeMap == &gameData.baseMap) {
        ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 250, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Always);

        ImGui::Begin("Storage");

        for (const auto &[type, amount]: storage.storedOres) {
            ImGui::Text("%s: %d", BlockRegistry[type].name, amount);
        }

        ImGui::End();
    }

#pragma endregion

    return true;
}
