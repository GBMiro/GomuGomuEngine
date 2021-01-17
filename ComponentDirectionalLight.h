#ifndef COMPONENT_DIRECTIONAL_LIGHT_H
#define COMPONENT_DIRECTIONAL_LIGHT_H
#include "ComponentLight.h"
class ComponentDirectionalLight :public ComponentLight {

private:
	float3 direction;
public:
	ComponentDirectionalLight(GameObject* go, float3 direction, int debugLineAmount = 30);
	~ComponentDirectionalLight();
	void Update()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void OnNewParent(const GameObject* prevParent, const  GameObject* newParent)override;
	void SendValuesToShadingProgram(const unsigned& programID, int id = 0) const override;
	void LoadFromJSON(const rapidjson::Value& component) override;
	void WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc);
	void CreateDebugLines() { GenerateDebugLines(); }
	void SetDirection(const float3& newDirection) { direction = newDirection; }
private:
	void GenerateDebugLines()override;

};

#endif