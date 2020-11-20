#pragma once
#include "Window.h"
class ConfigurationWindow : public Window
{
public:
	ConfigurationWindow(std::string name, int windowID);
	~ConfigurationWindow();

	void Draw();
};

