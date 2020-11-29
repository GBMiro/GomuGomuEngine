#pragma once
#include <string>
class Window
{
public:
	Window(std::string name, int windowID);
	virtual ~Window();

	virtual void Draw() = 0;

public:
	std::string name;
	bool active;
	int windowID;
};

