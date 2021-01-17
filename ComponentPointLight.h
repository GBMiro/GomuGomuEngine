#ifndef COMPONENT_POINT_LIGHT_H
#define COMPONENT_POINT_LIGHT_H
#include "ComponentLight.h"

class ComponentPointLight :public ComponentLight {
private:
	void GenerateDebugLines()override;
public:
	float constantAtt, linearAtt, quadraticAtt;

public:
	ComponentPointLight(GameObject* anOwner, float3 pos = float3::zero, float anInt = 1.0f, float3 aColor = float3::one, float cAtt = 1.0f, float lAtt = 0.01, float qAtt = 0.00016, int debugLineCount = 30);
	~ComponentPointLight();
	void Update()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void SendValuesToShadingProgram(const unsigned& programID, int id = 0) const override;
	void LoadFromJSON(const rapidjson::Value& component) override;
	void ComponentPointLight::WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;
	void CreateDebugLines() { GenerateDebugLines(); }
	//void OnNewParent(GameObject* prevParent, GameObject* newParent)override;
	//void OnTransformModified()override;
};

#endif

