#ifndef COMPONENT_LIGHT_H_
#define COMPONENT_LIGHT_H_
#include "Component.h"
#include "../MathGeoLib/Math/float3.h"
#include "ShadingProgram.h"


class ComponentLight :public Component {

public:
	float3 lightColor;
	float lightIntensity;
public:
	enum LightType { DIRECTIONAL, POINT, SPOT };
private:
	LightType type;

protected:
	int debugLineAmount;
	std::vector<float3>debugLines;

protected:
	virtual void GenerateDebugLines();

public:
	ComponentLight(GameObject* go, LightType type = LightType::DIRECTIONAL, float3 aColor = float3::zero, float anInt = 1.0f, int aDebugLineAmount = 20);
	~ComponentLight();
	void DrawOnEditor()override;
	LightType GetLightType() const { return type; }
	virtual void SendValuesToShadingProgram(const unsigned& programID, int id = 0)const;
	void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc);

private:
	virtual void WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) = 0;

};

#endif