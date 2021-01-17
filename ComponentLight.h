#ifndef COMPONENT_LIGHT_H_
#define COMPONENT_LIGHT_H_
#include "Component.h"
#include "../MathGeoLib/Math/float3.h"
#include "ShadingProgram.h"


class ComponentLight :public Component {

public:
	enum LightType { DIRECTIONAL, POINT, SPOT };
	float3 lightColor;
	float lightIntensity;
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
	void Enable() override;
	void Disable()override;
	void OnEnable()override;
	void OnDisable()override;
private:
	virtual void WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) = 0;

};

#endif