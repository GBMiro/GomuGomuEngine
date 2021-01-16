#include "ComponentLight.h"
#include "GameObject.h"
#include <imgui.h>
#include <GL/glew.h>
#include "Leaks.h"
#include "Application.h"
#include "ModuleScene.h"

void ComponentLight::GenerateDebugLines() {

}


ComponentLight::ComponentLight(GameObject* go, LightType type, float3 aColor, float anInt, int aDebugLineAmount) :Component(ComponentType::CTLight, go), type(type), lightColor(aColor), lightIntensity(anInt), debugLineAmount(aDebugLineAmount) {
	App->scene->AddLightComponent(this);
}

ComponentLight::~ComponentLight() {
	if (App->scene != nullptr)
		App->scene->RemoveLightComponent(this);
}

void ComponentLight::DrawOnEditor() {
	ImGui::InputFloat("Intensity", &lightIntensity);
	ImGui::InputFloat3("Color", lightColor.ptr());
}

void ComponentLight::SendValuesToShadingProgram(const unsigned& programID, int id)const {
	//Implemented on children, abstract method, could be deleted?
}

void ComponentLight::WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	component.AddMember("Component Type", GetType(), alloc);
	component.AddMember("UUID", GetUUID(), alloc);
	component.AddMember("ParentUUID", owner->GetUUID(), alloc);
	component.AddMember("Light Component Type", GetLightType(), alloc);

	rapidjson::Value lightColor(rapidjson::kArrayType);
	lightColor.PushBack(this->lightColor.x, alloc);
	lightColor.PushBack(this->lightColor.y, alloc);
	lightColor.PushBack(this->lightColor.z, alloc);
	component.AddMember("Light Color", lightColor, alloc);
	component.AddMember("Light Intensity", lightIntensity, alloc);
	WriteLightTypeJSON(component, alloc);
}

void ComponentLight::Enable() {
	Component::Enable();
}


void ComponentLight::Disable() {
	Component::Disable();
}

void ComponentLight::OnEnable() {
	App->scene->AddLightComponent(this);
}

void ComponentLight::OnDisable() {
	App->scene->RemoveLightComponent(this);
}