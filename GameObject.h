#pragma once
#include <string>
#include <vector>
#include "MathGeoLib/MathGeoLib.h"
#include "Component.h"

class Component;

class GameObject {
public:
	bool active; //TODO
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

private:
	AABB globalAABB;

public:
	//boolean for ctransform
	GameObject(GameObject* parent, const char* name);
	GameObject(GameObject* parent, const char* name, const float3& position, const Quat& rotation, const float3& scale);
	~GameObject();

	void Update();

	//TO DO
	void Draw();

	Component* CreateComponent(ComponentType type, int additionalParam = 0);

	void ChangeParent(GameObject* parent);
	bool IsAChild(const GameObject* gameObject) const;

	const char* GetName() const;

	void GenerateAABB();

	const AABB& GetAABB() const;

	Component* GetComponentOfType(ComponentType type) const;
	Component* GetComponentInChildrenOfType(ComponentType type);
	void GetComponentsInChildrenOfType(ComponentType type, std::vector<Component*>& components) const;

	void OnTransformChanged();
	void RemoveFromParent();
	void DrawGizmos();

	uint32_t GetUUID() { return UUID; }

	void GetAllChilds(std::vector<GameObject*>& children) const;
	void WriteToJSON(rapidjson::Value& gameObject, rapidjson::Document::AllocatorType& alloc);
	void RemoveParticularComponent(Component* c);
	void SetActive(bool should);
	bool Active()const;

private:
	uint32_t UUID;
};

