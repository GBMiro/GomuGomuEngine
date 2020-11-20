#include "MonitorWindow.h"
#include "imgui.h"
#include "SDL/include/SDL.h"


MonitorWindow::MonitorWindow(std::string name, int windowID) : Window(name, windowID), fps_log(MAX_SIZE), ms_log(MAX_SIZE) {
	hw.cpuCores = SDL_GetCPUCount();
	hw.cpuCache = SDL_GetCPUCacheLineSize();
	hw.RAM = SDL_GetSystemRAM() / (float)1024.f;
}

MonitorWindow::~MonitorWindow() {
}

void MonitorWindow::Draw() {

	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	static char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Miliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##miliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	ImGui::End();
}

void MonitorWindow::addData(float deltaTime) {
	
	if (fps_log.size() == MAX_SIZE) {
		for (unsigned i = 0; i < fps_log.size() - 1; ++i) {
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
		fps_log[fps_log.size() - 1] = 1 / (float)deltaTime;
		ms_log[ms_log.size() - 1] = deltaTime * 1000.0f;
	}
	else {
		fps_log.push_back(1 / (float)deltaTime);
		ms_log.push_back(deltaTime * 1000.0f);
	}
}
