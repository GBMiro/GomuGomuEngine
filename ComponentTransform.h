#pragma once
#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/float4x4.h"

class aiNode;

class ComponentTransform : public Component {

public:
	ComponentTransform(ComponentType type, const aiNode* node);
	~ComponentTransform();

	void Enable();
	void Update();
	void Disable();

	void DrawOnEditor();

public:
	float4x4 transform;

	float3 position;
	float3 scaling;
	Quat rotation;
};

