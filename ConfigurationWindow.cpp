#include "ConfigurationWindow.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Model.h"
#include "MathGeoLib/Math/float3.h"

ConfigurationWindow::ConfigurationWindow(std::string name, int windowID) : Window(name, windowID) {
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::Draw() {

	if (!active) return;

	static bool windowResizable = true;
	static bool fullscreen, windowBorderless = false;
	static int width;
	static int height;
	static float brightness = 1.0f;

	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui::Begin("Configuration");
	if (ImGui::CollapsingHeader("Input")) {}
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
		static bool WRAP, MAXMIN, MIPMAPS;
		if (ImGui::Checkbox("WRAP Mode", &WRAP));
		if (ImGui::Checkbox("MAX/MIN Filter", &MAXMIN)) App->model->setMinMaxFilter(MAXMIN);
		if (ImGui::Checkbox("Mipmap", &MIPMAPS));
	}
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) App->window->setFlag(SDL_WINDOW_FULLSCREEN, fullscreen); ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &windowResizable)) App->window->setFlag(SDL_WINDOW_RESIZABLE, windowResizable); ImGui::SameLine();
		if (ImGui::Checkbox("Borderless", &windowBorderless)) App->window->setFlag(SDL_WINDOW_BORDERLESS, windowBorderless);
		if (ImGui::SliderInt("Screen width", &width, 100, 1920)) App->window->setWindowSize(width, height);
		if (ImGui::SliderInt("Screen height", &height, 100, 1080)) App->window->setWindowSize(width, height);
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f")) App->window->setBrightness(brightness);
	}
	ImGui::End();
}
