#pragma once
#include <stdint.h>
#include "rapidjson/document.h"

enum ComponentType {
	CTTransform,
	CTMeshRenderer,
	CTCamera,
	CTLight
};

class GameObject;

class Component {

public:
	Component(ComponentType type, GameObject* parent);
	virtual ~Component();

	virtual void Enable() = 0;
	virtual void Update() = 0;
	virtual void Disable() = 0;

	virtual void OnDisable() = 0;
	virtual void OnEnable() = 0;


	virtual void DrawOnEditor() = 0;
	virtual void OnNewParent(const GameObject* oldParent, const GameObject* newParent) {};
	virtual void OnTransformChanged() {};
	virtual void DrawGizmos() {};
	ComponentType GetType() const;

	uint32_t GetUUID() const { return UUID; }

	virtual void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {}
	virtual void LoadFromJSON(const rapidjson::Value& component) {}

public:
	GameObject* owner;
	bool enabled = true;
	bool Enabled()const;
private:
	ComponentType type;
	uint32_t UUID;
};

