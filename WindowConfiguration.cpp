#include "WindowConfiguration.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "Model.h"
#include "MathGeoLib/Math/float3.h"
#include "Point.h"
#include "GL/glew.h"
#include "Leaks.h"

constexpr char* filterMinModes[] = { "Linear", "Nearest", "Linear mipmap linear", "Linear mipmap nearest", "Nearest mipmap linear", "Nearest mipmap nearest" };
constexpr char* filterMagModes[] = { "Linear", "Nearest" };
constexpr char* wrapModes[] = { "Clamp to borde", "Clamp", "Repeat", "Mirrored repeat" };

WindowConfiguration::WindowConfiguration(std::string name, int windowID) : Window(name, windowID) {
}

WindowConfiguration::~WindowConfiguration()
{
}

void WindowConfiguration::Draw() {

	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}

	static bool windowResizable = true;
	static bool fullscreen, windowBorderless = false;
	static int width, height;
	static float brightness = 1.0f;
	static float3 up, front, right, pos;
	static float zNear, zFar, aspectRatio, fov;
	static iPoint mouse;

	SDL_GetWindowSize(App->window->window, &width, &height);

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		up = App->camera->getUpVector();
		front = App->camera->getFrontVector();
		right = App->camera->getRightVector();
		pos = App->camera->getCameraPosition();
		aspectRatio = App->camera->getAspectRatio();
		fov = App->camera->getFOV();
		App->camera->getPlanes(&zNear, &zFar);
		if (ImGui::InputFloat3("Up", &up[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		if (ImGui::InputFloat3("Front", &front[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		if (ImGui::InputFloat3("Right", &right[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		ImGui::NewLine();
		if (ImGui::InputFloat3("Position", &pos[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		ImGui::NewLine();
		if (ImGui::SliderFloat("Movement Speed", &App->camera->cameraSpeed, 1.0f, MaxMovSpeed));
		if (ImGui::SliderFloat("Rotation Speed", &App->camera->angleSpeed, 1.0f, MaxRotSpeed));
		if (ImGui::SliderFloat("Zoom Speed", &App->camera->zoomSpeed, 1.0f, MaxZoomSpeed));
		ImGui::NewLine();
		if (ImGui::SliderFloat("zNear", &zNear, 0.1f, MaxZNear)) App->camera->setPlanes(zNear, zFar);
		if (ImGui::SliderFloat("zFar", &zFar, 0.1f, MaxZFar)) App->camera->setPlanes(zNear, zFar);
		if (ImGui::SliderFloat("FOV", &fov, 1.0f, MaxFOV)) App->camera->SetFOV(fov);
		if (ImGui::InputFloat("Aspect Ratio", &aspectRatio, 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly));
	}
	if (ImGui::CollapsingHeader("Input")) {
		mouse = App->input->GetMousePosition();
		float2 pos = float2(mouse.x, mouse.y);
		if (ImGui::InputFloat2("Mouse position", &pos[0], "%.0f", ImGuiInputTextFlags_ReadOnly));
	} // Mouse Position
	if (ImGui::CollapsingHeader("Renderer")) {
		static float gridColor[3] = { 0.0f, 0.0f, 1.0f };
		static float backgroundColor[3] = { 0.1f, 0.1f, 0.1f };
		float currentColor[3];
		App->renderer->getGridColor(currentColor);
		ImGui::ColorEdit3("Grid Color", gridColor);
		if (currentColor != gridColor) {
			App->renderer->setGridColor(gridColor);
		}
		App->renderer->getBackgroundColor(currentColor);
		ImGui::ColorEdit3("Background Color", backgroundColor);
		if (currentColor != backgroundColor) {
			App->renderer->setBackgroundColor(backgroundColor);
		}
	}
	if (ImGui::CollapsingHeader("Textures")) {
		static int indexMin = 0;
		static int indexMax = 0;
		static int indexWrap = 0;
		if (ImGui::BeginCombo("Min Filter", filterMinModes[indexMin])) {
			for (unsigned i = 0; i < IM_ARRAYSIZE(filterMinModes); ++i) {
				const bool is_selected = (indexMin == i);
				if (ImGui::Selectable(filterMinModes[i], is_selected)) {
					indexMin = i;
					ImGui::SetItemDefaultFocus();
					//App->model->setMinFilter(indexMin);
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Mag Filter", filterMagModes[indexMax])) {
			for (unsigned i = 0; i < IM_ARRAYSIZE(filterMagModes); ++i) {
				const bool is_selected = (indexMax == i);
				if (ImGui::Selectable(filterMagModes[i], is_selected)) {
					indexMax = i;
					ImGui::SetItemDefaultFocus();
					//App->model->setMagFilter(indexMax);
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("WRAP Mode", wrapModes[indexWrap])) {
			for (unsigned i = 0; i < IM_ARRAYSIZE(wrapModes); ++i) {
				const bool is_selected = (indexWrap == i);
				if (ImGui::Selectable(wrapModes[i], is_selected)) {
					indexWrap = i;
					ImGui::SetItemDefaultFocus();
					//App->model->setWrapMode(indexWrap);
				}
			}
			ImGui::EndCombo();
		}
	}
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) App->window->setFlag(SDL_WINDOW_FULLSCREEN_DESKTOP, fullscreen); ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &windowResizable)) App->window->setFlag(SDL_WINDOW_RESIZABLE, windowResizable); ImGui::SameLine();
		if (ImGui::Checkbox("Borderless", &windowBorderless)) App->window->setFlag(SDL_WINDOW_BORDERLESS, windowBorderless);
		if (!fullscreen) {
			if (ImGui::SliderInt("Screen width", &width, 100, 1920)) App->window->setWindowSize(width, height);
			if (ImGui::SliderInt("Screen height", &height, 100, 1080)) App->window->setWindowSize(width, height);
		}
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f")) App->window->setBrightness(brightness);
	}
	ImGui::End();
}
