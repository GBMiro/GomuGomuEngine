#include "ComponentLight.h"
#include "GameObject.h"
#include <imgui.h>
#include <GL/glew.h>
ComponentLight::ComponentLight(GameObject* go, LightType type, float3 aColor, float anInt) :Component(ComponentType::CTLight, go), type(type), lightColor(aColor), lightIntensity(anInt) {

}

ComponentLight::~ComponentLight() {

}

void ComponentLight::DrawOnEditor() {
	ImGui::InputFloat("Intensity", &lightIntensity);
	ImGui::InputFloat3("Color", lightColor.ptr());
}
