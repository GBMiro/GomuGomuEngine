#include "WindowAbout.h"
#include "imgui.h"
#include "Leaks.h"

WindowAbout::WindowAbout(std::string name, int windowID) : Window(name, windowID) {
	active = false;
}

WindowAbout::~WindowAbout() {
}

void WindowAbout::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	ImGui::Text("GomuGomu Engine is being developed in the UPC's Masters Advanced Programming for AAA video games.");
	ImGui::Text("This version is for the second assignment and uses a MIT license.");
	ImGui::Text("Authors: Guillem Burgués Miró - https://github.com/GBMiro/ - David Sierra González - https ://github.com/devildrake");
	ImGui::End();
}
