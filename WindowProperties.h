#pragma once
#include "Window.h"
#include <vector>

class Mesh;

class WindowProperties : public Window
{
public:
	WindowProperties(std::string name, int windowID);
	~WindowProperties();

	void Draw();
	void cleanProperties();

public:
	std::vector<Mesh*> meshes;
	std::vector<unsigned> textures;
};

