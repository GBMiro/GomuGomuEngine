#include "WindowScene.h"
#include "imgui.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"

WindowScene::WindowScene(std::string name, int windowID) : Window(name, windowID) {
	flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollWithMouse;
}

WindowScene::~WindowScene() {
}

void WindowScene::Draw() {

	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active, flags)) {
		ImGui::End();
		return;
	}
	ImVec2 size = ImGui::GetWindowSize();
	App->camera->SetAspectRatio(size.x / (float)size.y);
	if (ImGui::IsWindowHovered()) App->editor->SetGameWindowStatus(true);
	else App->editor->SetGameWindowStatus(false);
	//LOG("Window Hovered %d, Focus: %d", App->editor->GetGameWindowStatus(), ImGui::IsWindowFocused());
	ImGui::Image((ImTextureID)App->renderer->getFrameTexture(), size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}
