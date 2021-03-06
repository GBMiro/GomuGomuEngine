#include "WindowConsole.h"
#include "Leaks.h"

WindowConsole::WindowConsole(std::string name, int windowID) : Window(name, windowID) {
	autoScroll = true;
	buffer.clear();
}

WindowConsole::~WindowConsole() {
	buffer.clear();
}

void WindowConsole::AddLog(const char* fmt, ...) IM_FMTARGS(2) {

	int old_size = buffer.size();
	va_list args;
	va_start(args, fmt);
	buffer.appendfv(fmt, args);
	va_end(args);
	autoScroll = true;
}

void WindowConsole::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::Selectable("Clear")) buffer.clear();
		ImGui::EndPopup();
	}

	ImGui::TextUnformatted(buffer.begin());
	if (autoScroll)
		ImGui::SetScrollHere(1.0f);
	autoScroll = false;

	ImGui::EndChild();
	ImGui::End();
}
