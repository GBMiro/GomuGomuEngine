#include "Window.h"

Window::Window(std::string name, int windowID)
{
	this->name = name;
	this->windowID = windowID;
	active = true;
	flags = 0;
}

Window::~Window()
{
}
