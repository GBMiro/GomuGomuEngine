#pragma once
#include "Window.h"
#include "imgui.h"

class WindowConsole : public Window
{

public:
	WindowConsole(std::string name, int windowID);
	~WindowConsole();

	void Draw();

	void addLog(const char* fmt, ...) IM_FMTARGS(2);

private:
	ImGuiTextBuffer buffer;
	bool autoScroll;
};

