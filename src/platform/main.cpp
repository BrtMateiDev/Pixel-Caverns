#include <raylib.h>
#include "gameMain.h"

int main() {
#if PRODUCTION_BUILD==1
	SetTraceLogLevel(LOG_NONE); //disables raylib console output
#endif
#pragma region initializations
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(GetScreenWidth(), GetScreenHeight(), "Pixel Caverns");
	InitAudioDevice(); //audio setup
	SetExitKey(KEY_NULL);
	SetTargetFPS(144);
	//ToggleFullscreen();

#pragma endregion

	if (!initGame()) {
		return 0;
	}

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		bool keepRunning = updateGame();

		EndDrawing();

		if (!keepRunning) {
			break;
		}
	}

	CloseAudioDevice();
	CloseWindow();

	return 0;
}
