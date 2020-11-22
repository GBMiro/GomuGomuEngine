#pragma once
#include "Window.h"
#include <vector>

class Mesh;

class PropertiesWindow : public Window
{
public:
	PropertiesWindow(std::string name, int windowID);
	~PropertiesWindow();

	void Draw();
	void cleanProperties();

public:
	std::vector<Mesh*> meshes;
	std::vector<unsigned> textures;
};

