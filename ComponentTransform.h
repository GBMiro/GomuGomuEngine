#pragma once
#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

class ComponentTransform : public Component {

public:
	ComponentTransform(ComponentType type);
	~ComponentTransform();

	void Enable();
	void Update();
	void Disable();

public:
	float3 position;
	float3 scale;
	Quat rotation;
};

