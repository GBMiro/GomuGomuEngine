#include "ConfigurationWindow.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleWindow.h"

ConfigurationWindow::ConfigurationWindow(std::string name, int windowID) : Window(name, windowID) {
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::Draw() {

	if (!active) return;

	static bool windowResizable = true;
	static bool fullscreen, windowBorderless = false;
	static int width = SCREEN_WIDTH;
	static int height = SCREEN_HEIGHT;
	static float brightness = 1.0f;

	ImGui::Begin("Configuration");
	if (ImGui::CollapsingHeader("Input")) {}
	if (ImGui::CollapsingHeader("Renderer")) {}
	if (ImGui::CollapsingHeader("Textures")) {}
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
