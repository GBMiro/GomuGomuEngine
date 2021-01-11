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
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void OnNewParent(GameObject* prevParent, GameObject* newParent)override;
	void OnTransformChanged()override;
	Frustum& GetFrustum();
	void ComponentCamera::SetUpFrustum(float nearDistance, float farDistance);


};

#endif