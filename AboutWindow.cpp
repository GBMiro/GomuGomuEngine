#include "AboutWindow.h"
#include "imgui.h"

AboutWindow::AboutWindow(std::string name, int windowID) : Window(name, windowID)
{
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	ImGui::Text("GomuGomu Engine is being developed in the UPC's Masters Advanced Programming for AAA video games.");
	ImGui::End();
}
