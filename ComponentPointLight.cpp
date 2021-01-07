#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "../GameObject.h"
#include <imgui.h>
#include "../Application.h"
#include "../ModuleRender.h"
#include <GL/glew.h>
#include "ModuleDebugDraw.h"
#include "MathGeoLib/MathGeoLib.h"

ComponentPointLight::ComponentPointLight(GameObject* anOwner, float3 pos, float anInt, float3 aColor, float cAtt, float lAtt, float qAtt, int debugLineCount) :ComponentLight(anOwner, ComponentLight::LightType::POINT, aColor, anInt, debugLineCount), constantAtt(cAtt), linearAtt(lAtt), quadraticAtt(qAtt) {
	if (owner != nullptr) {
		ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
		if (transform != nullptr) {
			transform->SetPosition(pos);
		} else {
			transform = (ComponentTransform*)owner->CreateComponent(ComponentType::CTTransform);
			transform->SetPosition(pos);
		}
	}
	GenerateDebugLines();
}

ComponentPointLight::~ComponentPointLight() {

}

void ComponentPointLight::DrawOnEditor() {
	if (ImGui::CollapsingHeader("Point Light")) {
		ComponentLight::DrawOnEditor();

		if (ImGui::InputFloat("Constant Attenuation", &constantAtt)) {}
		if (ImGui::InputFloat("Lienar Attenuation", &linearAtt)) {}
		if (ImGui::InputFloat("Quadratic Attenuation", &quadraticAtt)) {}

	}
}

void ComponentPointLight::DrawGizmos() {

	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	for (int i = 0; i < debugLines.size(); ++i) {
		float3 position = transform->globalPosition;
		App->debugDraw->DrawLine(position, position + debugLines[i], float3::one);
	}
	//App->debugDraw->Draw
}


void ComponentPointLight::Enable() {

}

void ComponentPointLight::Update() {

}

void ComponentPointLight::Disable() {
}

void ComponentPointLight::GenerateDebugLines() {

	debugLines.clear();

	debugLines.reserve(debugLineAmount);
	math::LCG lcg;
	for (int i = 0; i < debugLineAmount; i++) {
		debugLines.push_back(float3::RandomDir(lcg));
	}
}

 void ComponentPointLight::SendValuesToShadingProgram(const unsigned& program) const {
	ComponentTransform* pointTransform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	glUniform3fv(glGetUniformLocation(program, "pointLight.position"), 1, (const float*)&pointTransform->globalPosition);
	glUniform3fv(glGetUniformLocation(program, "pointLight.color"), 1, (const float*)lightColor.ptr());
	glUniform3f(glGetUniformLocation(program, "pointLight.attenuation"), constantAtt, linearAtt, quadraticAtt);
	glUniform1f(glGetUniformLocation(program, "pointLight.intensity"), lightIntensity);
}