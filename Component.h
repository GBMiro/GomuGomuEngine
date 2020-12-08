#pragma once

enum ComponentType {
	TRANSFORM,
	RENDERER,
	CAMERA
};

class GameObject;

class Component {

public:
	Component(ComponentType type);
	virtual ~Component();

	virtual void Enable() = 0;
	virtual void Update() = 0;
	virtual void Disable() = 0;

	virtual void DrawOnEditor() {};

	ComponentType GetType() const;

public:
	GameObject* parent = nullptr;
	ComponentType type;
	bool active = false;
};

