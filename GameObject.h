#pragma once
#include <string>
#include <vector>
#include "MathGeoLib/Math/float4x4.h"
#include "Component.h"

class Component;

class GameObject {

public:
	GameObject(GameObject* parent, const char* name);
	GameObject(GameObject* parent, const char* name, const float3& position, const Quat& rotation, const float3& scale);
	~GameObject();

	void Update();

	void CleanUp();
	Component* CreateComponent(ComponentType type);

	void ChangeParent(GameObject* parent);
	bool IsAChild(const GameObject* gameObject) const;

	const char* GetName() const;

	AABB GetAABB() const;
	void GetChildsAABB(std::vector<AABB>& aabb) const;

	Component* GetComponentByType(ComponentType type) const;

	void OnTransformChanged();


public:
	std::string name;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
};

