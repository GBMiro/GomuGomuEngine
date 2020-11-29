#include "WindowAbout.h"
#include "imgui.h"
#include "Leaks.h"

WindowAbout::WindowAbout(std::string name, int windowID) : Window(name, windowID)
{
}

WindowAbout::~WindowAbout()
{
}

void WindowAbout::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	ImGui::Text("GomuGomu Engine is being developed in the UPC's Masters Advanced Programming for AAA video games.");
	ImGui::Text("This version is for the first assignment and uses a MIT license.");
	ImGui::End();
}
