#ifndef COMPONENT_CAMERA_H
#define COMPONENT_CAMERA_H
#include "Component.h"
#include "../MathGeoLib/Geometry/Frustum.h"
#include "../MathGeoLib/Math/float3x3.h"

class ComponentCamera :public Component {
private:
	Frustum frustum;
public:
	ComponentCamera(GameObject* anOwner, float aNearPDistance, float aFarPDistance);
	~ComponentCamera();
	void Enable() override;
	void Update()override;
	void Disable()override;
	void OnEnable()override;
	void OnDisable()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void OnNewParent(const GameObject* prevParent, const GameObject* newParent)override;
	void OnTransformChanged()override;
	Frustum& GetFrustum();
	void ComponentCamera::SetUpFrustum(float nearDistance, float farDistance);

	void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc);
	void LoadFromJSON(const rapidjson::Value& component);



};

#endif