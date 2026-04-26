#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <fstream>
#include <cmath>
#include "gameMain.h"
#include "assetManager.h"
#include "helpers.h"
#include "worldGenerator.h"
#include "physics.h"
#include "entities/slime.h"
#include "entityIdHolder.h"
#include "player.h"
#include "playerBase/storage.h"

struct GameData {
    GameMap worldMap;
    GameMap baseMap;
    GameMap *activeMap = nullptr;

    Camera2D camera = {};

    int creativeSelectedBlock = Block::dirt;

    Vector2 selectionStart = {};
    Vector2 selectionEnd = {};

    Player player;

    EntityHolder entities;
    EntityHolder *activeEntities = nullptr;
} gameData;

AssetManager assetManager;

//functions for spawning
void spawnSlime(Vector2 position) {
    Slime slime;

    slime.physics.teleport(position);

    auto id = gameData.entities.idHolder.getEntityIdAndIncrement();

    gameData.entities.entities[id] = std::make_unique<Slime>(slime); //syntax for the usage of unique (smart) pointers
}

bool initGame() {
    assetManager.loadAll();

    oracle.init(1);
    gameData.worldMap.usesOracle = true;
    gameData.baseMap.usesOracle = false; //Turning off the generation for the base

    generatePlayerBase(gameData.baseMap);

    gameData.activeMap = &gameData.worldMap;
    gameData.activeEntities = &gameData.entities;

    gameData.camera.target = {20.f, 120.f};
    gameData.camera.rotation = 0.0f;
    gameData.camera.zoom = 100.0f;

    gameData.player.teleport({20, 0});
    gameData.player.physics.transform.w = assetManager.player.width * pixel;
    gameData.player.physics.transform.h = assetManager.player.height * pixel;

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
    //if (IsKeyDown(KEY_W)) gameData.player.physics.transform.pos.y -= PLAYER_SPEED * dt;
    if (IsKeyDown(KEY_A)) gameData.player.physics.transform.pos.x -= PLAYER_SPEED * dt;
    //if (IsKeyDown(KEY_S)) gameData.player.physics.transform.pos.y += PLAYER_SPEED * dt;
    if (IsKeyDown(KEY_D)) gameData.player.physics.transform.pos.x += PLAYER_SPEED * dt;

    if (IsKeyDown(KEY_SPACE)) gameData.player.physics.jump(10);
#pragma endregion

#pragma region entities
    //player
    gameData.player.physics.applyGravity();
    gameData.player.physics.updateForces(dt);
    gameData.player.physics.resolveConstraints(*gameData.activeMap);
    gameData.camera.target = gameData.player.physics.transform.pos;
    gameData.player.physics.updateFinal();

    //entities
    if (gameData.activeEntities) {
        std::ranlux24_base rng(std::random_device{}());

        for (auto it = gameData.entities.entities.begin(); it != gameData.entities.entities.end();) {
            EntityUpdateData updateData{
                gameData.player.getPosition(),
                rng,
                gameData.entities,
                it->first
            };

            bool shouldKill = false;

            if (!it->second->update(dt, updateData) || it->second->life <= 0) shouldKill = true;

            if (shouldKill) it = gameData.entities.entities.erase(it); //returns the next valid iterator
            else {
                it->second->physics.applyGravity();
                it->second->physics.updateForces(dt);
                it->second->physics.resolveConstraints(*gameData.activeMap);
                it->second->physics.updateFinal();

                ++it;
            }
        }
    }

#pragma endregion

#pragma region mouse and keyboard actions
    if (IsKeyPressed(KEY_MINUS)) {
        gameData.creativeSelectedBlock--;
        if (gameData.creativeSelectedBlock <= 0) gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1;
    }
    if (IsKeyPressed(KEY_EQUAL)) {
        gameData.creativeSelectedBlock++;
        if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) gameData.creativeSelectedBlock = 1;
    }

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);;

    int blockX = floor(worldPos.x);
    int blockY = floor(worldPos.y);

    if (gameData.activeMap == &gameData.worldMap) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto &b = gameData.worldMap.getBlock(blockX, blockY);
            if (b.type) {
                gameData.player.inventory.mineOre(&b);
                b.type = Block::air;
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
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
            if (b.type != Block::air) {
                DrawTexturePro(
                    assetManager.textures,
                    getTextureAtlas(b.type, 0, 32, 32),
                    {(float) x, (float) y, 1, 1},
                    {0, 0},
                    0.0f,
                    WHITE
                );
            }
        }
#pragma endregion

#pragma region drawing entities
    if (gameData.activeEntities)
        for (auto &e: gameData.entities.entities)
            e.second->render(assetManager);

#pragma endregion

#pragma region drawing the player
    gameData.player.render_tail(assetManager);
    gameData.player.render(assetManager);
    //Drawing the player's hitbox
    //DrawRectangleLinesEx(gameData.player.physics.transform.getAABB(), 0.1, {20, 101, 250, 120});
#pragma endregion

#pragma region visualizing block selection
    if (gameData.activeMap == &gameData.worldMap)
        DrawTexturePro(
            assetManager.frame,
            {0, 0, (float) assetManager.frame.width, (float) assetManager.frame.height},
            {(float) blockX, (float) blockY, 1, 1},
            {0, 0},
            0.f,
            WHITE
        );
#pragma endregion

    EndMode2D();

#pragma region imgui_windows
    //This disables the generation of ImGui.ini and in-game dragging
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);

    ImGui::Begin("Dev tools");

    ImGui::SliderFloat("Camera zoom", &gameData.camera.zoom, 1, 175);
    ImGui::SliderFloat("Player speed", &PLAYER_SPEED, 5, 100);
    if (gameData.activeEntities) {
        if (ImGui::Button("Spawn Slime")) spawnSlime({40, 0});
        if (ImGui::Button("Hurt a Slime")) {
            for (auto &e: gameData.entities.entities) {
                if (e.second->getEntityType() == EntityType_Slime) {
                    e.second->life -= 3;
                    break;
                }
            }
        }
    }
    if (ImGui::Button("Teleport to base")) {
        gameData.activeMap = &gameData.baseMap;
        gameData.activeEntities = nullptr;
        gameData.player.teleport({5, 2});
    }
    if (ImGui::Button("Teleport to mine")) {
        gameData.activeMap = &gameData.worldMap;
        gameData.activeEntities = &gameData.entities;
        gameData.player.teleport({20, 10});
    }

    if (gameData.activeMap == &gameData.baseMap) {
        ImGui::Separator();
        if (ImGui::Button("Deposit ores")) storage.depositOres(gameData.player.inventory.minedOres);
    }

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
        ImGui::Text("%s: %d", getBlockName(type), amount);
    }
    ImGui::End();

    //Base storage window
    if (gameData.activeMap == &gameData.baseMap) {
        ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 250, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Always);

        ImGui::Begin("Storage");

        for (const auto &[type, amount]: storage.storedOres) {
            ImGui::Text("%s: %d", getBlockName(type), amount);
        }

        ImGui::End();
    }

#pragma endregion

    return true;
}

void closeGame() {
    std::ofstream fout("/home/matei/Workspace/C++/Pixel Caverns/resources/crashLogs/" "log.txt");
    fout << "CLOSED!!";
    fout.close();
}
