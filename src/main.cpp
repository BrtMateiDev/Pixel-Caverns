#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "window name");

	rlImGuiSetup(true);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		rlImGuiBegin();

		DrawText("Congrats! You created your first window!", 190, 200, 20, RED);

		ImGui::Begin("test");

		ImGui::Text("hello");
		ImGui::Button("button");

		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}