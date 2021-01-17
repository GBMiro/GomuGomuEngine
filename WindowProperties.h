#pragma once
#include "Window.h"
#include <vector>

class WindowProperties : public Window
{
public:
	WindowProperties(std::string name, int windowID);
	~WindowProperties();

	void Draw();
};

