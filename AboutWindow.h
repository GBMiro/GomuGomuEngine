#pragma once
#include "Window.h"
class AboutWindow : public Window
{
public:
	AboutWindow(std::string name, int windowID);
	~AboutWindow();

	void Draw();
};

