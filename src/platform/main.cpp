#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>
#include "gameMain.h"

int main() {
#if PRODUCTION_BUILD==1
	SetTraceLogLevel(LOG_NONE); //disables raylib console output
#endif
#pragma region initializations
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "Pixel Caverns");
	//SetExitKey(KEY_NULL); //disables exiting on esc
	SetTargetFPS(240);
	rlImGuiSetup(true);

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontGlobalScale = 1.3;
#pragma endregion

	if (!initGame()) {
		return 0;
	}

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		rlImGuiBegin();

#pragma region imgui_docking
		/*
		 ==DISABLED==
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);
		*/
#pragma endregion

		//This disables the generation of ImGui.ini and in-game dragging
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = nullptr;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

		if (!updateGame()) {
			CloseWindow();
		}

		rlImGuiEnd();
		EndDrawing();
	}

	CloseWindow();
	closeGame();
	rlImGuiShutdown();

	return 0;
}
