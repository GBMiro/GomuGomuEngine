#include "ComponentLight.h"
#include "GameObject.h"
#include <imgui.h>
#include <GL/glew.h>


void ComponentLight::GenerateDebugLines() {

}


ComponentLight::ComponentLight(GameObject* go, LightType type, float3 aColor, float anInt, int aDebugLineAmount) :Component(ComponentType::CTLight, go), type(type), lightColor(aColor), lightIntensity(anInt), debugLineAmount(aDebugLineAmount) {

}

ComponentLight::~ComponentLight() {

}

void ComponentLight::DrawOnEditor() {
	ImGui::InputFloat("Intensity", &lightIntensity);
	ImGui::InputFloat3("Color", lightColor.ptr());
}

void ComponentLight::SendValuesToShadingProgram(const unsigned& programID)const {
	//Implemented on children, abstract method, could be deleted?
}