#pragma once

enum ComponentType {
	CTTransform,
	CTMeshRenderer,
	CTCamera
};

class GameObject;

class Component {

public:
	Component(ComponentType type, GameObject* parent);
	virtual ~Component();

	virtual void Enable() = 0;
	virtual void Update() = 0;
	virtual void Disable() = 0;

	virtual void DrawOnEditor() {};

	virtual void OnTransformChanged() {};
	virtual void DrawGizmos() {};
	ComponentType GetType() const;

public:
	GameObject* owner;
	ComponentType type;
	bool active = false;
};

