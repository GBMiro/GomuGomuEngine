#include "ComponentDirectionalLight.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleDebugDraw.h"
#include "GameObject.h"
#include "imgui.h"
#include <GL/glew.h>
#include "Leaks.h"

ComponentDirectionalLight::ComponentDirectionalLight(GameObject* go, float3 direction, int debugLineCount) :ComponentLight(go, LightType::DIRECTIONAL, float3::one, 1.0f, debugLineCount), direction(direction) {
	GenerateDebugLines();
}

ComponentDirectionalLight::~ComponentDirectionalLight() {

}

void ComponentDirectionalLight::Update() {

}


void ComponentDirectionalLight::DrawOnEditor() {

	bool dummyEnabled = enabled;
	ImGui::PushID(this);

	if (ImGui::Checkbox("", &dummyEnabled)) {
		if (dummyEnabled) {
			Enable();
		} else {
			Disable();
		}
	}

	ImGui::SameLine();
	ImGui::PopID();

	if (ImGui::CollapsingHeader("Directional Light")) {
		ComponentLight::DrawOnEditor();
		if (ImGui::InputFloat3("Direction", direction.ptr())) {}
	}

}

void ComponentDirectionalLight::DrawGizmos() {
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	for (int i = 0; i < debugLines.size(); ++i) {
		float3 position = transform->Position();
		App->debugDraw->DrawLine(position + debugLines[i], position + debugLines[i] + (transform->Rotation() * direction), float3::one);
	}
}

void ComponentDirectionalLight::OnNewParent(const GameObject* prevParent, const GameObject* newParent) {

}

void ComponentDirectionalLight::GenerateDebugLines() {
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);

	debugLines.clear();

	debugLines.reserve(debugLineAmount);
	math::LCG lcg;
	for (int i = 0; i < debugLineAmount; i++) {

		debugLines.push_back(float3::RandomSphere(lcg, transform->Position(), 1.0f));

		//debugLines.push_back(float3::RandomDir(lcg));
	}
}

void ComponentDirectionalLight::SendValuesToShadingProgram(const unsigned& program, int id) const {
	glUniform3fv(glGetUniformLocation(program, "dirLight.color"), 1, (const float*)lightColor.ptr());
	glUniform3fv(glGetUniformLocation(program, "dirLight.direction"), 1, (const float*)direction.ptr());

	glUniform1f(glGetUniformLocation(program, "dirLight.intensity"), lightIntensity);
}

void ComponentDirectionalLight::LoadFromJSON(const rapidjson::Value& component) {
	if (component.HasMember("Enabled")) {
		enabled = component["Enabled"].GetBool();
	} else {
		enabled = true;
	}

	float3 direction;
	direction.x = component["Direction"][0].GetFloat();
	direction.y = component["Direction"][1].GetFloat();
	direction.z = component["Direction"][2].GetFloat();

	float3 lightColor;
	lightColor.x = component["Light Color"][0].GetFloat();
	lightColor.y = component["Light Color"][1].GetFloat();
	lightColor.z = component["Light Color"][2].GetFloat();

	float intensity;
	intensity = component["Light Intensity"].GetFloat();

	SetDirection(direction);
	this->lightColor = lightColor;
	this->lightIntensity = intensity;

	CreateDebugLines();
}

void ComponentDirectionalLight::WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {

	rapidjson::Value direction(rapidjson::kArrayType);
	direction.PushBack(this->direction.x, alloc);
	direction.PushBack(this->direction.y, alloc);
	direction.PushBack(this->direction.z, alloc);
	component.AddMember("Direction", direction, alloc);
}


//
//void ComponentDirectionalLight::OnTransformModified() {
//	//Maybe calculate debug rays
//}