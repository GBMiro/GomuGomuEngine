#ifndef COMPONENT_POINT_LIGHT_H
#define COMPONENT_POINT_LIGHT_H
#include "ComponentLight.h"

class ComponentPointLight :public ComponentLight {
public:
	float constantAtt, linearAtt, quadraticAtt;

public:
	ComponentPointLight(GameObject* anOwner, float3 pos = float3::zero, float anInt = 1.0f, float3 aColor = float3::one, float cAtt = 0, float lAtt = 0, float qAtt = 0);
	~ComponentPointLight();
	void Enable() override;
	void Update()override;
	void Disable()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	//void OnNewParent(GameObject* prevParent, GameObject* newParent)override;
	//void OnTransformModified()override;
};

#endif

