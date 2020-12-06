#pragma once
#include "Window.h"

class GameObject;

class WindowGameObjectHierarchy : public Window {

public:
	
	WindowGameObjectHierarchy(std::string name, int windowID);
	~WindowGameObjectHierarchy();

	void Draw();

	void DrawGameObjectHierarchy(GameObject* gameObject);
};

