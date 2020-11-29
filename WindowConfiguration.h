#pragma once
#include "Window.h"
class WindowConfiguration : public Window
{
public:
	WindowConfiguration(std::string name, int windowID);
	~WindowConfiguration();

	void Draw();
};

