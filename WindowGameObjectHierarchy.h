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

public:
	GameObject* gameObjectSelected = nullptr;
	GameObject* gameObjectDrop = nullptr;
};

