#include <raylib.h>
#include "gameMain.h"
#include <fstream>

struct GameData {

    float posX=100;
    float posY=100;

}gameData;

bool initGame() {
    return true;
}

bool updateGame() {
    float dt=GetFrameTime(); //this is delta time which calculates the amount of time between the last and current frames
    if (dt>1.f/5) dt=1.f/5; //dt is capped at 1/5 in case of lag spikes

    if (IsKeyDown(KEY_A)) gameData.posX-=200.f*dt;
    if (IsKeyDown(KEY_D)) gameData.posX+=200.f*dt;
    if (IsKeyDown(KEY_W)) gameData.posY-=200.f*dt;
    if (IsKeyDown(KEY_S)) gameData.posY+=200.f*dt;


    DrawRectangle(gameData.posX, gameData.posY, 50, 50, RED);

    return true;
}

void closeGame() {
    std::ofstream fout ("/home/matei/Workspace/C++/Pixel Caverns/resources/crashLogs/" "log.txt");
    fout<<"CLOSED!!";
    fout.close();
}