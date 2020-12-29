#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "../GameObject.h"
#include <imgui.h>
#include "../Application.h"
#include "../ModuleRender.h"
#include <GL/glew.h>
#include "ModuleDebugDraw.h"

ComponentPointLight::ComponentPointLight(GameObject* anOwner, float3 pos, float anInt, float3 aColor, float cAtt, float lAtt, float qAtt) :ComponentLight(anOwner, ComponentLight::LightType::POINT, aColor, anInt), constantAtt(cAtt), linearAtt(lAtt), quadraticAtt(qAtt) {
	if (owner != nullptr) {
		ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
		if (transform != nullptr) {
			transform->SetPosition(pos);
		} else {
			transform = (ComponentTransform*)owner->CreateComponent(ComponentType::CTTransform);
			transform->SetPosition(pos);
		}
	}
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
	//App->debugDraw->Draw
}


void ComponentPointLight::Enable() {

}

void ComponentPointLight::Update() {

}

void ComponentPointLight::Disable() {
}