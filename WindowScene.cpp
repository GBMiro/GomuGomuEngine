#include "WindowScene.h"
#include "imgui.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "Globals.h"

WindowScene::WindowScene(std::string name, int windowID) : Window(name, windowID) {
	flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollWithMouse;
}

WindowScene::~WindowScene() {

}

void WindowScene::Draw() {

	//Push and Pop style var generate a crash when minimization happens
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active, flags)) {
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}

	//This should only happen onWindowResize()
	windowSize = ImGui::GetWindowSize();
	mousePosInWindow = ImGui::GetIO().MousePos;
	windowPos = ImGui::GetWindowPos();

	mousePosInWindow.x -= windowPos.x;
	mousePosInWindow.y -= windowPos.y;

	mousePosInWindow.y -= 16;

	App->camera->SetAspectRatio(windowSize.x / (float)windowSize.y);
		if (ImGui::IsWindowHovered()) App->editor->SetGameWindowStatus(true);
		else App->editor->SetGameWindowStatus(false);

	ImGui::Image((ImTextureID)App->renderer->GetFrameTexture(), windowSize, ImVec2(0, 1), ImVec2(1, 0));

	App->editor->ManageGizmos();

	ImGui::End();
	ImGui::PopStyleVar();

}

const ImVec2& WindowScene::GetSize()const {
	return windowSize;
}

const ImVec2& WindowScene::GetMousePosInWindow()const {
	return mousePosInWindow;
}

const ImVec2& WindowScene::GetPosition()const {
	return windowPos;
}