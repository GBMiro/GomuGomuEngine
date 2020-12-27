#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui.h"
#include "Leaks.h"


ComponentTransform::ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling) : Component(ComponentType::CTTransform, parent) {

	this->position = position;
	this->rotation = rotation;
	this->scaling = scaling;

	UpdateLocalTransform();
	UpdateGlobaltransform();
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
	if (owner->parent) {
		if (ImGui::CollapsingHeader("Transformations")) {
			if (ImGui::DragFloat3("Position", &position.x, 0.1f, -90.0f, 90.0f, "%.3f")) {
				UpdateLocalTransform();
				owner->OnTransformChanged();
			}
			if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f, -90.0f, 90.0f, "%.3f")) UpdateLocalTransform();
			if (ImGui::DragFloat3("Scale", &scaling.x, 0.1f, -90.0f, 90.0f, "%.3f")) {
				UpdateLocalTransform();
				owner->OnTransformChanged();
			}
		}
	}
}

void ComponentTransform::UpdateLocalTransform() {
	localTransform = float4x4::FromTRS(position, rotation, scaling);
}

void ComponentTransform::UpdateGlobaltransform() {
	if (owner) {
		if (owner->parent) {
			ComponentTransform* cTransform = (ComponentTransform*)owner->parent->GetComponentOfType(CTTransform);
			if (cTransform) {
				globalTransform = cTransform->globalTransform * localTransform;
			}
		}
		else {
			globalTransform = float4x4::identity;
		}
	}
}

void ComponentTransform::SetGlobalTransform() {
	ComponentTransform* cTransform = (ComponentTransform*)owner->parent->GetComponentOfType(CTTransform);
	if (cTransform) {
		localTransform = globalTransform * cTransform->globalTransform.Inverted();
		localTransform.Decompose(position, rotation, scaling);
		owner->OnTransformChanged();
	}
}

void ComponentTransform::OnTransformChanged() {
	UpdateGlobaltransform();
}
