#include <raylib.h>
#include <fstream>
#include <cmath>
#include "gameMain.h"
#include "assetManager.h"
#include "gameMap.h"
#include "helpers.h"

std::uint16_t currentBlock=Block::gold;

struct GameData {
    GameMap gameMap;
    Camera2D camera;
}gameData;

AssetManager assetManager;

bool initGame() {
    assetManager.loadAll();

    gameData.gameMap.create(30, 10);

    gameData.gameMap.getBlockUnsafe(0,0).type=Block::dirt;
    gameData.gameMap.getBlockUnsafe(1,1).type=Block::grass;
    gameData.gameMap.getBlockUnsafe(2,2).type=Block::goldBlock;
    gameData.gameMap.getBlockUnsafe(3,3).type=Block::glass;
    gameData.gameMap.getBlockUnsafe(4,4).type=Block::platform;


    gameData.camera.target={0.0};
    gameData.camera.rotation=0.0f;
    gameData.camera.zoom=100.0f;

    return true;
}

bool updateGame() {
    float dt=GetFrameTime(); //this is delta time which calculates the amount of time between the last and current frames
    if (dt>1.f/5) dt=1.f/5; //dt is capped at 1/5 in case of lag spikes

    gameData.camera.offset={GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};

    ClearBackground({75, 75, 150, 255});

#pragma region camera movement
    float mouseWheel=GetMouseWheelMove();
    if (mouseWheel>0 && gameData.camera.zoom<=200.f) gameData.camera.zoom+=10.f;
    if (mouseWheel<0 && gameData.camera.zoom>10.f) gameData.camera.zoom-=10.f;

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mouseDelta=GetMouseDelta();

        //Note: The camera has to be moved in the opposite direction
        gameData.camera.target.x-=mouseDelta.x/gameData.camera.zoom;
        gameData.camera.target.y-=mouseDelta.y/gameData.camera.zoom;
    }
#pragma endregion

#pragma region mouse actions
    if (IsKeyPressed(KEY_ONE)) currentBlock=Block::dirt;
    if (IsKeyPressed(KEY_TWO)) currentBlock=Block::stone;
    if (IsKeyPressed(KEY_THREE)) currentBlock=Block::gold;

    Vector2 worldPos=GetScreenToWorld2D(GetMousePosition(), gameData.camera);;

    int blockX=(int)floor(worldPos.x);
    int blockY=(int)floor(worldPos.y);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        auto b=gameData.gameMap.getBlockSafe(blockX, blockY);
        if (b) *b={};
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        auto b=gameData.gameMap.getBlockSafe(blockX, blockY);
        if (b) b->type=currentBlock;
    }

    if (IsMouseButtonDown)
#pragma endregion

    BeginMode2D(gameData.camera);

    for (int y=0; y<gameData.gameMap.h; ++y)
        for (int x=0; x<gameData.gameMap.w; ++x) {
            auto &b=gameData.gameMap.getBlockUnsafe(x,y);
            if (b.type!=Block::air) {

                DrawTexturePro(
                    assetManager.textures,
                    getTextureAtlas(b.type, 0, 32, 32),
                    {(float)x, (float)y, 1, 1},
                    {0,0},
                    0.0f,
                    WHITE
                );
            }
        }

#pragma region visualizing block selection
    DrawTexturePro(
        assetManager.frame,
        {0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height},
        {(float)blockX, (float)blockY, 1, 1},
        {0, 0},
        0.f,
        WHITE
    );
#pragma endregion

    EndMode2D();

    return true;
}

void closeGame() {
    std::ofstream fout ("/home/matei/Workspace/C++/Pixel Caverns/resources/crashLogs/" "log.txt");
    fout<<"CLOSED!!";
    fout.close();
}