#pragma once
#include "Component.h"
#include <string>

class Mesh;

class ComponentMeshRenderer : public Component {

public:
	ComponentMeshRenderer(GameObject* parent);
	~ComponentMeshRenderer();

	void Enable();
	void Update();
	void Disable();

	void Draw();

	void SetTextureName(std::string name) { textureName = name; }

public:
	Mesh* mesh = nullptr;
	std::string textureName = "";
};

