#pragma once
#include "Window.h"
#include <string>
class WindowScene : public Window {

public:

	WindowScene(std::string name, int windowID);
	~WindowScene();

	void Draw() override;

};

