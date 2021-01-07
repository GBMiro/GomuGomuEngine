#pragma once
#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/float4x4.h"

class ComponentTransform : public Component {

public:
	ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling);
	~ComponentTransform();

	void Enable() override;
	void Update() override;
	void Disable() override;

	void DrawOnEditor() override;

	void UpdateLocalMatrix();
	void UpdateGlobalMatrix();

	void OnNewParent(GameObject* oldParent, GameObject* newParent)override;
	void Reset();

	void SetPosition(float3 newGlobalPos);
	void DrawGizmos()override;

private:
	void UpdateLocalValues();
	void UpdateGlobalValues();
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

	float3 globalPosition;
	float3 globalScale;
	Quat globalRotation;

	float3 globalForward;
	float3 globalUp;
	float3 globalRight;


private:
	float3 oldLocalPosition;
	float3 oldLocalScale;
	Quat   oldLocalRotation;
	float3 oldRotDummy;

};

