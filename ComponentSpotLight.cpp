#include "ComponentSpotLight.h"
#include "ComponentTransform.h"
#include "../GameObject.h"
#include <imgui.h>
#include "../Application.h"
#include "../ModuleRender.h"
#include <GL/glew.h>
#include "ModuleDebugDraw.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Leaks.h"

void ComponentSpotLight::GenerateDebugLines() {

	debugLines.clear();

	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	float3 position = transform->Position();

	debugLines.reserve(debugLineAmount);
	float3 forward = transform->Forward();

	for (int i = 0; i < debugLineAmount / 2; i++) {
		float4 rotationAxis = float4(forward.x, forward.y, forward.z, 1.0f);
		Quat rotation = Quat(rotationAxis, DegToRad((360 / debugLineAmount)) * i * 2);

		float3 myVector = rotation * transform->Up();


		float4 aux = rotationAxis;

		float4 aux2 = float4(myVector.x, myVector.y, myVector.z, 1.0f);

		float4 secondaryRotationAxis = aux2.Cross(aux);

		Quat secondaryRotation = Quat(secondaryRotationAxis, DegToRad(90 - minAngle));

		myVector = secondaryRotation * myVector;

		debugLines.push_back(myVector);
	}


	for (int i = 0; i < debugLineAmount / 2; i++) {
		float4 rotationAxis = float4(forward.x, forward.y, forward.z, 1.0f);
		Quat rotation = Quat(rotationAxis, DegToRad((360 / debugLineAmount)) * i * 2);

		float3 myVector = rotation * transform->Up();

		float4 aux = rotationAxis;

		float4 aux2 = float4(myVector.x, myVector.y, myVector.z, 1.0f);

		float4 secondaryRotationAxis = aux2.Cross(aux);

		Quat secondaryRotation = Quat(secondaryRotationAxis, DegToRad(90 - maxAngle));

		myVector = secondaryRotation * myVector;

		debugLines.push_back(myVector * 2.0f);

	}

}

ComponentSpotLight::ComponentSpotLight(GameObject* anOwner, float3 pos, float anInt, float3 aColor, float cAtt, float lAtt, float qAtt, int debugLineCount) :ComponentLight(anOwner, ComponentLight::LightType::POINT, aColor, anInt, debugLineCount), constantAtt(cAtt), linearAtt(lAtt), quadraticAtt(qAtt) {
	if (owner != nullptr) {
		ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
		if (transform != nullptr) {
			transform->SetPosition(pos);
		} else {
			transform = (ComponentTransform*)owner->CreateComponent(ComponentType::CTTransform);
			transform->SetPosition(pos);
		}
	}

	minAngle = 15.0f;
	maxAngle = 45.0f;

	GenerateDebugLines();
}

ComponentSpotLight::~ComponentSpotLight() {

}
void ComponentSpotLight::Enable() {
}

void ComponentSpotLight::Update() {
}

void ComponentSpotLight::Disable() {
}

void ComponentSpotLight::DrawOnEditor() {
	if (ImGui::CollapsingHeader("Spot Light")) {

		ComponentLight::DrawOnEditor();

		if (ImGui::InputFloat("Constant Attenuation", &constantAtt)) {}
		if (ImGui::InputFloat("Lienar Attenuation", &linearAtt)) {}
		if (ImGui::InputFloat("Quadratic Attenuation", &quadraticAtt)) {}

		if (ImGui::DragFloat("min Angle", &minAngle)) {
			GenerateDebugLines();
		}
		if (ImGui::DragFloat("max Angle", &maxAngle)) {
			GenerateDebugLines();
		}
	}
}

void ComponentSpotLight::DrawGizmos() {
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	for (int i = 0; i < debugLines.size(); ++i) {
		float3 position = transform->Position();
		App->debugDraw->DrawLine(position, position + debugLines[i], float3::one);
	}
}

void ComponentSpotLight::WriteLightTypeJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	
	rapidjson::Value direction(rapidjson::kArrayType);
	direction.PushBack(this->direction.x, alloc);
	direction.PushBack(this->direction.y, alloc);
	direction.PushBack(this->direction.z, alloc);
	component.AddMember("Direction", direction, alloc);
	
	component.AddMember("Constant Att", constantAtt, alloc);
	component.AddMember("Linear Att", linearAtt, alloc);
	component.AddMember("Quadratic Att", quadraticAtt, alloc);

	component.AddMember("Min Angle", minAngle, alloc);
	component.AddMember("Max Angle", maxAngle, alloc);
}
