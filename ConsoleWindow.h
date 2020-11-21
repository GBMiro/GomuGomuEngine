#pragma once
#include "Window.h"
#include "imgui.h"

class ConsoleWindow : public Window
{

public:
	ConsoleWindow(std::string name, int windowID);
	~ConsoleWindow();

	void Draw();

	void addLog(const char* fmt, ...) IM_FMTARGS(2);

private:
	ImGuiTextBuffer buffer;
	bool autoScroll;
};

