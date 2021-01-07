#include "ComponentDirectionalLight.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleDebugDraw.h"
#include "GameObject.h"
#include "imgui.h"
#include <GL/glew.h>

ComponentDirectionalLight::ComponentDirectionalLight(GameObject* go, float3 direction, int debugLineCount) :ComponentLight(go, LightType::DIRECTIONAL, float3::one, 1.0f, debugLineCount), direction(direction) {
	GenerateDebugLines();
}

ComponentDirectionalLight::~ComponentDirectionalLight() {

}

void ComponentDirectionalLight::Enable() {

}

void ComponentDirectionalLight::Update() {

}

void ComponentDirectionalLight::Disable() {

}

void ComponentDirectionalLight::DrawOnEditor() {
	if (ImGui::CollapsingHeader("Directional Light")) {
		ComponentLight::DrawOnEditor();
		if (ImGui::InputFloat3("Direction", direction.ptr())) {}

	}

}

void ComponentDirectionalLight::DrawGizmos() {
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	for (int i = 0; i < debugLines.size(); ++i) {
		float3 position = transform->globalPosition;
		App->debugDraw->DrawLine(position + debugLines[i], position + debugLines[i] + (transform->globalRotation * direction), float3::one);
	}
}

void ComponentDirectionalLight::OnNewParent(GameObject* prevParent, GameObject* newParent) {

}

void ComponentDirectionalLight::GenerateDebugLines() {
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);

	debugLines.clear();

	debugLines.reserve(debugLineAmount);
	math::LCG lcg;
	for (int i = 0; i < debugLineAmount; i++) {

		debugLines.push_back(float3::RandomSphere(lcg, transform->globalPosition, 1.0f));

		//debugLines.push_back(float3::RandomDir(lcg));
	}
}

void ComponentDirectionalLight::SendValuesToShadingProgram(const unsigned& program) const {
	glUniform3fv(glGetUniformLocation(program, "dirLight.color"), 1, (const float*)lightColor.ptr());
	glUniform3fv(glGetUniformLocation(program, "dirLight.direction"), 1, (const float*)direction.ptr());

	glUniform1f(glGetUniformLocation(program, "dirLight.intensity"), lightIntensity);
}


//
//void ComponentDirectionalLight::OnTransformModified() {
//	//Maybe calculate debug rays
//}