#pragma once
#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/float4x4.h"

class ComponentTransform : public Component {

public:
	ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling);
	~ComponentTransform();

	void Enable();
	void Update();
	void Disable();

	void DrawOnEditor();

	void UpdateLocalTransform();
	void UpdateGlobaltransform();

	void SetGlobalTransform();

	void OnTransformChanged();

public:
	float4x4 localTransform;
	float4x4 globalTransform;

	float3 position;
	float3 scaling;
	Quat rotation;
};

