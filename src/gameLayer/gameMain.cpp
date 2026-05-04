#include <raylib.h>
#include <imgui.h>
#include "gameMain.h"
#include "assetManager.h"
#include "helpers.h"
#include "worldGenerator.h"
#include "physics.h"
#include "player.h"
#include "playerBase/storage.h"

static bool hitbox_toggle = false;
static bool storage_toggle = false;
static bool canMove = true;

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

void tp_mine() {
    gameData.activeMap = &gameData.worldMap;
    gameData.player.teleport({20, 0});
    gameData.player.facingDirection = 1;
    gameData.player.speed = 7;
    //gameData.activeEntities = &gameData.entities;
}

void tp_base() {
    gameData.activeMap = &gameData.baseMap;
    gameData.player.teleport({28, 7});
    gameData.player.facingDirection = -1;
    gameData.player.speed = 5;
    //gameData.activeEntities = nullptr;
}

void open_storage() {
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 250, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Always);

    ImGui::Begin("Storage");

    for (const auto &[type, amount]: storage.storedOres) {
        ImGui::Text("%s: %d", BlockRegistry[type].name, amount);
    }

    ImGui::End();
}

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
    if (IsKeyDown(KEY_A) && canMove) {
        gameData.player.physics.transform.pos.x -= gameData.player.speed * dt;
        gameData.player.facingDirection = -1;
    }
    if (IsKeyDown(KEY_D) && canMove) {
        gameData.player.physics.transform.pos.x += gameData.player.speed * dt;
        gameData.player.facingDirection = 1;
    }

    if (IsKeyDown(KEY_SPACE) && gameData.activeMap == &gameData.worldMap) gameData.player.physics.jump(10);
#pragma endregion

#pragma region updating physics
    //player
    gameData.player.physics.applyGravity();
    gameData.player.physics.updateForces(dt);
    gameData.player.physics.resolveConstraints(*gameData.activeMap);

    gameData.camera.target = gameData.player.physics.transform.pos;

    if (gameData.activeMap == &gameData.baseMap) {
        // Clamp X axis so we don't look past the walls
        float screenWorldWidth = GetScreenWidth() / gameData.camera.zoom;
        float minX = screenWorldWidth / 2.0f;
        float maxX = 30.0f - (screenWorldWidth / 2.0f);

        if (maxX < minX) {
            gameData.camera.target.x = 30.0f / 2.0f;
        } else {
            if (gameData.camera.target.x < minX) gameData.camera.target.x = minX;
            if (gameData.camera.target.x > maxX) gameData.camera.target.x = maxX;
        }

        // Clamp Y axis dynamically
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
        for (int y = startYView; y <= endYView; ++y)
            for (int x = startXView; x <= endXView; ++x) {
                auto &b = gameData.activeMap->getBlock(x, y);
                const auto &props = BlockRegistry[b.type];
                Rectangle destRect = {(float) x, (float) y, 1, 1};

                // 1. Background blocks
                DrawTexturePro(
                    assetManager.textures,
                    getTextureAtlas(b.bgTexture, 1, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                    destRect,
                    {0, 0},
                    0.0f,
                    WHITE
                );

                // 2. Blocks
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

                // 3. Ores
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

#pragma region drawing entities (DISABLED)
    // if (gameData.activeEntities)
    //     for (auto &e: gameData.entities.entities)
    //         e.second->render(assetManager);

#pragma endregion

#pragma region drawing the player
    gameData.player.render_tail(assetManager);
    gameData.player.render(assetManager);

    //Hitbox visualization
    if (hitbox_toggle) DrawRectangleLinesEx(gameData.player.physics.transform.getAABB(), 0.05, {20, 101, 250, 120});
#pragma endregion

#pragma region visualizing block selection and mining cracks
    if (gameData.activeMap == &gameData.worldMap && inRange) {
        // Draw the main selection highlight
        DrawTexturePro(
            assetManager.textures,
            getTextureAtlas(0, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
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


    // Coordinate System
    if (gameData.activeMap == &gameData.worldMap) {
        float buttonSize = 128;
        int buttonIndex = 1;
        Rectangle tpButton = {
            0,
            (float) GetScreenHeight() - buttonSize,
            buttonSize,
            buttonSize,
        };

        Vector2 mousePos = GetMousePosition();
        bool isHovered = CheckCollisionPointRec(mousePos, tpButton);
        bool isHeldDown = isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool isReleased = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        if (!isHovered) buttonIndex = 1;
        if (isHovered) buttonIndex = 2;
        if (isHeldDown) buttonIndex = 3;
        if (isReleased) {
            tp_base();
            //TODO: ADD CUTSCENE
        }
        DrawTexturePro(
            assetManager.textures,
            getTextureAtlas(buttonIndex, 3, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
            {0, (float) GetScreenHeight() - 128, 128, 128},
            {0, 0},
            0.f,
            WHITE
        );

        int playerX = (int) floorf(gameData.player.physics.transform.pos.x);
        int playerY = (int) floorf(gameData.player.physics.transform.pos.y);
        const char *coordText = TextFormat("(X: %d; Y: %d)", playerX, playerY);
        int textWidth = MeasureText(coordText, 40);
        DrawText(coordText, GetScreenWidth() / 2 - textWidth / 2, 15, 40, RAYWHITE);
    }

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
            }
        } else if (px >= 17 && px <= 22) {
            prompt = "Press [E] to use the pickaxe forge (NOT IMPLEMENTED)";

            if (IsKeyPressed(KEY_E)) {
                //TODO: ADD INTERACTION
            }
        } else if (px >= 25 && px <= 27) {
            prompt = "Press [E] to sleep (reset mine) (NOT IMPLEMENTED)";
            if (IsKeyPressed(KEY_E)) {
                //TODO: ADD CUTSCENE
            }
        } else if (px >= 28 && px <= 29) {
            prompt = "Press [E] to exit the base";

            if (IsKeyPressed(KEY_E)) {
                tp_mine();
            }
        }

        if (storage_toggle) open_storage();

        if (prompt != nullptr) {
            Vector2 screenPos = GetWorldToScreen2D({px, 4.0f}, gameData.camera);

            int textWidth = MeasureText(prompt, 40);
            DrawRectangle((int) screenPos.x - textWidth / 2 - 5, (int) screenPos.y - 155, textWidth + 10, 50,
                          {0, 0, 0, 150});
            DrawText(prompt, (int) screenPos.x - textWidth / 2, (int) screenPos.y - 150, 40, YELLOW);
        }
    }
#pragma endregion

#pragma region imgui_windows

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);

    ImGui::Begin("Dev tools");

    ImGui::SliderFloat("Camera zoom", &gameData.camera.zoom, 5, 250);
    ImGui::SliderFloat("Player speed", &gameData.player.speed, 1, 100);
    ImGui::SliderFloat("Pickaxe power", &gameData.player.pickaxePower, 1, 10);
    ImGui::Checkbox("Show player hitbox", &hitbox_toggle);

    if (gameData.activeMap == &gameData.baseMap) {
        if (ImGui::Button("Deposit ores")) storage.depositOres(gameData.player.inventory.minedOres);
    }

    ImGui::Text("FPS: %d", GetFPS());

    ImGui::End();

    //Player inventory window
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 250, GetScreenHeight() - 300), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Always);

    ImGui::Begin("Inventory");

    for (const auto &[type, amount]: gameData.player.inventory.minedOres) {
        ImGui::Text("%s: %d", BlockRegistry[type].name, amount);
    }
    ImGui::End();

#pragma endregion

    return true;
}
