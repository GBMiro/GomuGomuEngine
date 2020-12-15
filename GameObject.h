#pragma once
#include <string>
#include <vector>
#include "MathGeoLib/Math/float4x4.h"
#include "Component.h"

class aiNode;
class aiMesh;
class Component;
class ComponentTransform;
class ComponentMeshRenderer;

class GameObject {

public:
	GameObject(const char* name);
	~GameObject();

	void Update();

	void CleanUp();

	ComponentTransform* CreateTransformComponent(const aiNode* node);
	ComponentMeshRenderer* CreateMeshRendererComponent(const aiMesh* mesh);

	void UpdateGameObjectsTransform(const float4x4& parentTransform);

	const float4x4& GetTransformationMatrix() const;

	const char* GetName() const;

public:
	const char* name;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
	float4x4 globalTransform = float4x4::identity;
};

