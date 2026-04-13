#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <fstream>
#include <cmath>
#include "gameMain.h"
#include "assetManager.h"
#include "gameMap.h"
#include "helpers.h"
#include "worldGenerator.h"
#include "physics.h"
#include "entities/slime.h"
#include "entityIdHolder.h"

bool showImGui = true;

struct GameData {
    GameMap gameMap;
    Camera2D camera = {};

    int creativeSelectedBlock = Block::dirt;

    Vector2 selectionStart = {};
    Vector2 selectionEnd = {};

    PhysicalEntity player;

    EntityHolder entities;
} gameData;

AssetManager assetManager;

//quick function for spawning
void spawnSlime(Vector2 position) {
    Slime slime;
    slime.physics.teleport(position);
    auto id = gameData.entities.idHolder.getEntityIdAndIncrement();
    gameData.entities.entities[id] = std::make_unique<Slime>(slime); //syntax for the usage of unique (smart) pointers
}

bool initGame() {
    assetManager.loadAll();

    generateWorld(gameData.gameMap, 1);

    gameData.camera.target = {20.f, 120.f};
    gameData.camera.rotation = 0.0f;
    gameData.camera.zoom = 100.0f;

    gameData.player.teleport({20, 0});
    gameData.player.transform.w = 0.9f;
    gameData.player.transform.h = 1.8f;

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
    if (IsKeyDown(KEY_W)) gameData.player.transform.pos.y -= PLAYER_SPEED * dt;
    if (IsKeyDown(KEY_A)) gameData.player.transform.pos.x -= PLAYER_SPEED * dt;
    if (IsKeyDown(KEY_S)) gameData.player.transform.pos.y += PLAYER_SPEED * dt;
    if (IsKeyDown(KEY_D)) gameData.player.transform.pos.x += PLAYER_SPEED * dt;

    if (IsKeyDown(KEY_SPACE)) gameData.player.jump(10);
#pragma endregion

#pragma region entities
    //player
    gameData.player.applyGravity();
    gameData.player.updateForces(dt);
    gameData.player.resolveConstraints(gameData.gameMap);
    gameData.camera.target = gameData.player.transform.pos;
    gameData.player.updateFinal();

    //entities
    std::ranlux24_base rng(std::random_device{}());

    EntityUpdateData updateData{
        gameData.player.getPosition(),
        rng,
    };

    for (auto &e: gameData.entities.entities) {
        //"second" means the entity key in the unordered map
        e.second->update(dt, updateData);

        e.second->physics.applyGravity();
        e.second->physics.updateForces(dt);
        e.second->physics.resolveConstraints(gameData.gameMap);
        e.second->physics.updateFinal();
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
    if (IsKeyPressed(KEY_SEVEN)) showImGui = !showImGui;

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);;

    int blockX = floor(worldPos.x);
    int blockY = floor(worldPos.y);

    if (!showImGui && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
        if (b) *b = {};
    } //checking ImGui so that you can't break blocks when moving the window

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
        if (b) b->type = gameData.creativeSelectedBlock;
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

    //Clamp means the variable can't have a value outside those bounds
    startXView = Clamp(startXView, 0, (float) gameData.gameMap.w - 1);
    endXView = Clamp(endXView, 0, (float) gameData.gameMap.w - 1);
    startYView = Clamp(startYView, 0, (float) gameData.gameMap.h - 1);
    endYView = Clamp(endYView, 0, (float) gameData.gameMap.h - 1);
#pragma endregion

#pragma region drawing the map
    for (int y = startYView; y <= endYView; ++y)
        for (int x = startXView; x <= endXView; ++x) {
            auto &b = gameData.gameMap.getBlockUnsafe(x, y);
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
    for (auto &e: gameData.entities.entities)
        e.second->render(assetManager);

#pragma endregion

#pragma region drawing the player
    Transform2D playerSprite = gameData.player.transform;

    playerSprite.w = 1;
    playerSprite.h = 2;
    //Moving the sprite so that the bottom of it matches the bottom of the hitbox
    playerSprite.pos.y -= (playerSprite.h - gameData.player.transform.h) / 2;

    DrawTexturePro(
        assetManager.player,
        {0, 0, (float) assetManager.player.width, (float) assetManager.player.height},
        playerSprite.getAABB(),
        {0, 0},
        0.0f,
        WHITE
    );

    //Drawing the player's hitbox
    //DrawRectangleLinesEx(gameData.player.transform.getAABB(), 0.1, {20, 101, 250, 120});
#pragma endregion

#pragma region visualizing block selection
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
    if (showImGui) {
        ImGui::Begin("Dev tools");

        ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 2, 150);
        ImGui::SliderFloat("Player speed:", &PLAYER_SPEED, 5, 100);

        if (ImGui::Button("Spawn Slime")) spawnSlime({40, 0});
        if (ImGui::Button("Reset player position")) gameData.player.teleport({20, 10});

        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Press 7 to disable/enable, F11 for fullscreen and ESC to close the game");
            ImGui::EndTooltip();
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
