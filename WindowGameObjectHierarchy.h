#pragma once
#include "Window.h"

class GameObject;

class WindowGameObjectHierarchy : public Window {

public:
	
	WindowGameObjectHierarchy(std::string name, int windowID);
	~WindowGameObjectHierarchy();

	void Draw();

	GameObject* GetGameObjectSelected() const { return gameObjectSelected; }

private:

	void DrawGameObjectHierarchy(GameObject* gameObject);

private:
	GameObject* gameObjectSelected = nullptr;
	GameObject* gameObjectDrop = nullptr;
};

