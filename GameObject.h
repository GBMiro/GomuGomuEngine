#pragma once
#include <string>
#include <vector>
#include "Component.h"

class aiNode;
class aiMesh;

class GameObject {

public:
	GameObject(const char* name);
	~GameObject();

	void Update();

	void CleanUp();

	Component* CreateTransformComponent(const aiNode* node);
	Component* CreateMeshRendererComponent(const aiMesh* mesh);

	const char* GetName() const;

public:
	const char* name;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
};

