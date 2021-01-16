#ifndef COMPONENT_SPOT_LIGHT_H
#define COMPONENT_SPOT_LIGHT_H
#include "ComponentLight.h"
#include "MathGeoLib/Math/float3.h"
class ComponentSpotLight :public ComponentLight {
private:
	void GenerateDebugLines()override;
public:
	float constantAtt, linearAtt, quadraticAtt;
	float3 direction;
	float minAngle, maxAngle;
public:
	ComponentSpotLight(GameObject* anOwner, float3 pos = float3::zero, float anInt = 1.0f, float3 aColor = float3::one, float cAtt = 1.0f, float lAtt = 0.01, float qAtt = 0.00016, int debugLineCount = 30);
	~ComponentSpotLight();
	void Update()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;
	//void OnNewParent(GameObject* prevParent, GameObject* newParent)override;
	//void OnTransformModified()override;
};

#endif

