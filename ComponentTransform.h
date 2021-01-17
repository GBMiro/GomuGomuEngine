#pragma once
#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/float4x4.h"

class ComponentTransform : public Component {
private:
	float4x4 localMatrix;
	float4x4 globalMatrix;
	float3 localPosition;
	float3 localScale;
	Quat localRotation;
public:
	ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling);
	~ComponentTransform();

	void Enable() override;
	void Update() override;
	void Disable() override;
	void OnEnable()override;
	void OnDisable()override;
	void DrawOnEditor() override;

	void UpdateLocalMatrix();
	void UpdateGlobalMatrix();

	void OnNewParent(const GameObject* oldParent, const  GameObject* newParent)override;
	void Reset();

	void SetPosition(float3 newGlobalPos);
	void DrawGizmos()override;

	void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;
	void SetLocalPosition(float3 newPos);
	void SetLocalRotation(Quat newRot);
	void SetLocalScale(float3 newScale);
	void SetLocalMatrix(float4x4 newLocalMat);
	void SetGlobalMatrix(float4x4 newGlobalMat);

	float3 LocalPosition() const { return localPosition; }
	Quat LocalRotation() const { return localRotation; }
	float3 LocalScale()	const { return localScale; }

	float3 Position();
	float3 Scale()const;
	Quat Rotation()const;

	float3 Forward()const;
	float3 Up()const;
	float3 Right()const;
	float3 Left()const;

	float3 LocalForward()const;
	float3 LocalUp()const;
	float3 LocalRight()const;
	float3 LocalLeft()const;

	float4x4 GetLocalMatrix()const { return localMatrix; }
	float4x4 GetGlobalMatrix()const { return globalMatrix; }


private:
	void UpdateLocalValues();
	void UpdateGlobalValues();
	float3 CalculateGlobalPosition()const;
	Quat CalculateGlobalRotation()const;
	float3 CalculateGlobalScale()const;


};

