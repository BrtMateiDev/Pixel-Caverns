#include <raylib.h>
#include "gameMain.h"
#include<fstream>
bool initGame() {
    return true;
}

bool updateGame() {
    DrawText("Congrats! You created your first window!", 190, 200, 20, RED);

    return true;
}

void closeGame() {
    std::ofstream fout ("/home/matei/Workspace/C++/Pixel Caverns/resources/crashLogs/" "log.txt");
    fout<<"CLOSED!!";
    fout.close();
}