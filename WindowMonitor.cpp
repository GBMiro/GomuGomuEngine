#include "WindowMonitor.h"
#include "imgui.h"
#include "SDL/include/SDL.h"
#include "GL/glew.h"
#include "DevIL/include/IL/il.h"
#include "Leaks.h"


WindowMonitor::WindowMonitor(std::string name, int windowID) : Window(name, windowID), fps_log(MAX_SIZE), ms_log(MAX_SIZE) {
	hw.cpuCores = SDL_GetCPUCount();
	hw.cpuCache = SDL_GetCPUCacheLineSize();
	hw.RAM = SDL_GetSystemRAM() / (float)1024.f;
}

WindowMonitor::~WindowMonitor() {
}

void WindowMonitor::Draw() {

	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	if (ImGui::CollapsingHeader("Hardware", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("CPU cores: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "CPU cores: %d (Cache: %dkb)", hw.cpuCores, hw.cpuCache);
		ImGui::Text("RAM: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f", hw.RAM);
		ImGui::Text("GPU: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s, %s", glGetString(GL_RENDERER), glGetString(GL_VENDOR));
	}
	if (ImGui::CollapsingHeader("Memory consumption", ImGuiTreeNodeFlags_DefaultOpen)) {
		static int availableKb, budgetKb, usageKb, reservedKb = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &budgetKb);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableKb);

		ImGui::Text("VRAM Budget: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f Mb", budgetKb / (float) 1024.0f);
		ImGui::Text("VRAM Usage: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f Mb", (budgetKb - availableKb) / (float) 1024.0f);
		ImGui::Text("VRAM Available: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f Mb", availableKb / (float)1024.0f);
		ImGui::Text("VRAM Reserved: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f Mb", float(0.0));
	
	}
	if (ImGui::CollapsingHeader("Metrics graph", ImGuiTreeNodeFlags_DefaultOpen)) {
		static char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Miliseconds %0.3f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##miliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
	if (ImGui::CollapsingHeader("Software versions", ImGuiTreeNodeFlags_DefaultOpen)) {
		static int ilVersion = ilGetInteger(IL_VERSION_NUM);
		SDL_version compiled;
		SDL_VERSION(&compiled);
		ImGui::Text("OpenGL: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));
		ImGui::Text("SDL: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);
		ImGui::Text("DevIL: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", ilVersion / 100, (ilVersion / 10) % 10, ilVersion % 10);
		ImGui::Text("Glew: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glewGetString(GLEW_VERSION));
	}
	ImGui::End();
}

void WindowMonitor::addData(float deltaTime) {
	
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
