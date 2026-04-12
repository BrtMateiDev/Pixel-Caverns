#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <cmath>
#include "gameMain.h"
#include "assetManager.h"
#include "gameMap.h"
#include "helpers.h"
#include "worldGenerator.h"
#include "physics.h"

std::uint16_t currentBlock=Block::gold;

struct GameData {
    GameMap gameMap;
    Camera2D camera;
}gameData;

AssetManager assetManager;

bool initGame() {
    assetManager.loadAll();

    generateWorld(gameData.gameMap, 1);

    gameData.camera.target={20.f, 120.f};
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

#pragma region camera view borders
    Vector2 topLeftView=GetScreenToWorld2D({0,0}, gameData.camera);
    Vector2 bottomRightView=GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, gameData.camera);

    int startXView = (int)floorf(topLeftView.x-1);
    int endXView = (int)floorf(topLeftView.x+1);
    int startYView = (int)floorf(topLeftView.y-1);
    int endYView = (int)floorf(topLeftView.y+1);

    startXView=Clamp(startXView, 0, (float)gameData.gameMap.w-1); //Clamp means the variable can't have a value outside those bounds
    endXView=Clamp(endXView, GetScreenWidth(), gameData.gameMap.w-1);
    startYView=Clamp(startXView, 0, gameData.gameMap.h-1);
    endYView=Clamp(endYView, 0, gameData.gameMap.h-1);
#pragma endregion

    for (int y=startYView; y<=endXView; ++y)
        for (int x=startXView; x<=endXView; ++x) {
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

#pragma region testing collisions
    //Notice: Remove the "/* */" comments to enable one test, but remember to disable the other

    Transform2D test;
    test.pos={20.5f, 120.5f}; //the position of the camera + 0.5 for the center
    test.w=1;
    test.h=1;

    Transform2D test2;
    test2.pos=worldPos;
    test2.w=1;
    test2.h=1;

    /*
    //TEST 1: CHECKING POINT COLLISION
    //(the worldPos variable was calculated earlier for the mouse position)
    if (test.intersectPoint(worldPos)) {
        DrawRectangleLinesEx(test.getAABB(), 0.1, GREEN);
    }
    else {
        DrawRectangleLinesEx(test.getAABB(), 0.1, BLUE);
    }
    */

    //TEST 2: CHECKING TRANSFORM COLLISION
    /*
    if (test.intersectTransform(test2)) {
        DrawRectangleLinesEx(test.getAABB(), 0.1, GREEN);
        DrawRectangleLinesEx(test2.getAABB(), 0.1, GREEN);
    }
    else {
        DrawRectangleLinesEx(test.getAABB(), 0.1, BLUE);
        DrawRectangleLinesEx(test2.getAABB(), 0.1, RED);
    }
    */

    //TEST 3: VISUALIZING THE PLAYER REACH
    if (Vector2Distance(test.pos, test2.pos)<=1.5f) DrawLineEx(test.pos, worldPos, 0.1, GREEN);
    else DrawLineEx(test.pos, worldPos, 0.1, RED);
    DrawCircleV(test.pos, 0.1, PURPLE);

#pragma endregion

    EndMode2D();

    return true;
}

void closeGame() {
    std::ofstream fout ("/home/matei/Workspace/C++/Pixel Caverns/resources/crashLogs/" "log.txt");
    fout<<"CLOSED!!";
    fout.close();
}