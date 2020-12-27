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

	void UpdateLocalMatrix();
	void UpdateGlobalMatrix();

	void OnNewParent(GameObject* oldParent, GameObject* newParent)override;
	void Reset();
	float3 CalculateGlobalPosition()const;
	Quat CalculateGlobalRotation()const;
	float3 CalculateGlobalScale()const;




public:
	bool enabled;
	float4x4 localMatrix;
	float4x4 globalMatrix;

	float3 localPosition;
	float3 localScale;
	Quat localRotation;



private:
	float3 oldLocalPosition;
	float3 oldLocalScale;
	Quat   oldLocalRotation;
	float3 oldRotDummy;

};

