#pragma once
#include "Window.h"
class WindowAbout : public Window
{
public:
	WindowAbout(std::string name, int windowID);
	~WindowAbout();

	void Draw();
};

