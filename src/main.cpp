#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>
#include <iostream>
int main()
{

#pragma region initializations
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "window name");

	rlImGuiSetup(true);

	ImGuiIO &io=ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontGlobalScale=1.3;
#pragma endregion

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		rlImGuiBegin();

#pragma region imgui_docking
		ImGui::PushStyleColor(ImGuiCol_WindowBg,{});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg,{});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);
#pragma endregion

		DrawText("Congrats! You created your first window!", 190, 200, 20, RED);

#pragma region imgui_windows
		ImGui::Begin("test");

		ImGui::Text("I'm surprised you're looking at such an old commit");

		ImGui::End();
#pragma endregion

		rlImGuiEnd();
		EndDrawing();
	}
	rlImGuiShutdown();
	CloseWindow();
	return 0;
}