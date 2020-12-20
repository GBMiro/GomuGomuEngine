#include "ComponentTransform.h"
#include "imgui.h"
#include "Leaks.h"


ComponentTransform::ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling) : Component(ComponentType::TRANSFORM, parent) {

	this->position = position;
	this->rotation = rotation;
	this->scaling = scaling;

	transform = float4x4::FromTRS(position, rotation, scaling);
}

ComponentTransform::~ComponentTransform() {
}

void ComponentTransform::Enable() {
	active = true;
}

void ComponentTransform::Update() {
}

void ComponentTransform::Disable() {
	active = false;
}

void ComponentTransform::DrawOnEditor() {
	if (ImGui::CollapsingHeader("Transformations")) {
		if (ImGui::DragFloat3("Position", &position.x, 0.1f, -90.0f, 90.0f, "%.3f")) transform = float4x4::FromTRS(position, rotation, scaling);
		if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f, -90.0f, 90.0f, "%.3f")) transform = float4x4::FromTRS(position, rotation, scaling);
		if (ImGui::DragFloat3("Scale", &scaling.x, 0.1f, -90.0f, 90.0f, "%.3f")) transform = float4x4::FromTRS(position, rotation, scaling);
	}
}
