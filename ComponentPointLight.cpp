#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "../GameObject.h"
#include <imgui.h>
#include "../Application.h"
#include "../ModuleRender.h"
#include <GL/glew.h>
#include "ModuleDebugDraw.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Leaks.h"

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
		float3 position = transform->Position();
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

void ComponentPointLight::SendValuesToShadingProgram(const unsigned& program, int id) const {
	ComponentTransform* pointTransform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);

	std::string pointLightShaderString = "pointLights[";
	pointLightShaderString += std::to_string(id);
	pointLightShaderString += "].";



	glUniform3fv(glGetUniformLocation(program, (pointLightShaderString + "position").c_str()), 1, (const float*)&pointTransform->Position());
	glUniform3fv(glGetUniformLocation(program, (pointLightShaderString + "color").c_str()), 1, (const float*)lightColor.ptr());
	glUniform3f(glGetUniformLocation(program, (pointLightShaderString + "attenuation").c_str()), constantAtt, linearAtt, quadraticAtt);
	glUniform1f(glGetUniformLocation(program, (pointLightShaderString + "intensity").c_str()), lightIntensity);
}

void ComponentPointLight::WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {

	component.AddMember("Constant Att", constantAtt, alloc);
	component.AddMember("Linear Att", linearAtt, alloc);
	component.AddMember("Quadratic Att", quadraticAtt, alloc);
}