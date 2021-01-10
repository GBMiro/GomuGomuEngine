#ifndef COMPONENT_DIRECTIONAL_LIGHT_H
#define COMPONENT_DIRECTIONAL_LIGHT_H
#include "ComponentLight.h"
class ComponentDirectionalLight :public ComponentLight {

private:
	float3 direction;
public:
	ComponentDirectionalLight(GameObject* go, float3 direction, int debugLineAmount = 30);
	~ComponentDirectionalLight();
	void Enable() override;
	void Update()override;
	void Disable()override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void OnNewParent(GameObject* prevParent, GameObject* newParent)override;
	void SendValuesToShadingProgram(const unsigned& programID) const override;

	//void OnTransformUpdated()override;
private:
	void GenerateDebugLines()override;

};

#endif