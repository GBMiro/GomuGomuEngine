#ifndef COMPONENT_SPOT_LIGHT_H
#define COMPONENT_SPOT_LIGHT_H

////////////////////////////////THIS CLASS IS NOT WORKING NOR BEING USED -> WIP//////////////////////////////
////////////////////////////////THIS CLASS IS NOT WORKING NOR BEING USED -> WIP//////////////////////////////
////////////////////////////////THIS CLASS IS NOT WORKING NOR BEING USED -> WIP//////////////////////////////
////////////////////////////////THIS CLASS IS NOT WORKING NOR BEING USED -> WIP//////////////////////////////
////////////////////////////////THIS CLASS IS NOT WORKING NOR BEING USED -> WIP//////////////////////////////
////////////////////////////////THIS CLASS IS NOT WORKING NOR BEING USED -> WIP//////////////////////////////

#include "ComponentLight.h"
#include "MathGeoLib/Math/float3.h"
class ComponentSpotLight :public ComponentLight {
private:
	void GenerateDebugLines()override;
public:
	float3 direction;
	float minAngle, maxAngle;
private:
	float constantAtt, linearAtt, quadraticAtt;

public:
	ComponentSpotLight(GameObject* anOwner, float3 pos = float3::zero, float anInt = 1.0f, float3 aColor = float3::one, float cAtt = 1.0f, float lAtt = 0.01, float qAtt = 0.00016, int debugLineCount = 30);
	~ComponentSpotLight();
	void Update()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;

	float GetConstantAtt()const { return constantAtt; }
	float GetLinearAtt()const { return linearAtt; }
	float GetQuadraticAtt()const { return quadraticAtt; }

	void GetConstantAtt(float newAtt) { constantAtt = newAtt; }
	void GetLinearAtt(float newAtt) { linearAtt = newAtt; }
	void GetQuadraticAt(float newAtt) { quadraticAtt = newAtt; }
};

#endif

