#include "ComponentTransform.h"
#include "imgui.h"
#include "assimp/scene.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Leaks.h"


ComponentTransform::ComponentTransform(ComponentType type, const aiNode* node) : Component(type) {

	position = float3(0.0);
	rotation = Quat::identity;
	scaling = float3(1.0);

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
