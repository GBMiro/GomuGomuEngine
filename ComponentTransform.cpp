#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleDebugDraw.h"
#include "Leaks.h"

ComponentTransform::ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling) : Component(ComponentType::CTTransform, parent) {

	this->localPosition = position;
	this->localRotation = rotation;
	this->localScale = scaling;

	globalMatrix = float4x4::identity;
	localMatrix = float4x4::identity;

	UpdateLocalValues();
}

ComponentTransform::~ComponentTransform() {

}

void ComponentTransform::Enable() {
	active = true;
}

void ComponentTransform::Update() {
	if (!enabled) return;

}


void ComponentTransform::SetLocalPosition(float3 newPos) {
	localPosition = newPos;
	UpdateLocalValues();
	owner->OnTransformChanged();
}

void ComponentTransform::SetLocalRotation(Quat newRot) {
	localRotation = newRot;
	UpdateLocalValues();
	owner->OnTransformChanged();
}

void ComponentTransform::SetLocalScale(float3 newScale) {
	localScale = newScale;
	UpdateLocalValues();
	owner->OnTransformChanged();
}

void ComponentTransform::UpdateGlobalValues() {
	UpdateGlobalMatrix();
}

float3 ComponentTransform::Position() {
	return globalMatrix.TranslatePart();
}

float3 ComponentTransform::Scale() {
	return globalMatrix.GetScale();
}

Quat ComponentTransform::Rotation() {

	//TO DO Scale must be controlled after being modified OR THIS MAY RETURN ERRORS


	float3x3 rotation = globalMatrix.RotatePart();

	rotation.Orthonormalize(0, 1, 2);

	return rotation.ToQuat();
}

float3 ComponentTransform::Forward() {
	return globalMatrix.WorldZ().Normalized();
}

float3 ComponentTransform::Up() {
	return globalMatrix.WorldY().Normalized();
}

float3 ComponentTransform::Right() {
	return  globalMatrix.WorldX().Normalized();
}

float3 ComponentTransform::Left() {
	return -Right();
}

float3 ComponentTransform::LocalForward() { return localMatrix.WorldZ().Normalized(); }
float3 ComponentTransform::LocalUp() { return localMatrix.WorldY().Normalized(); }
float3 ComponentTransform::LocalRight() { return localMatrix.WorldX().Normalized(); }
float3 ComponentTransform::LocalLeft() { return -LocalRight(); }

void ComponentTransform::UpdateLocalValues() {
	UpdateLocalMatrix();
	UpdateGlobalValues();
}

void ComponentTransform::Disable() {
	active = false;
}

void ComponentTransform::DrawOnEditor() {
	if (owner->parent) {
		if (ImGui::CollapsingHeader("Transformations")) {

			float3 localPositionDummy = localPosition;
			if (ImGui::DragFloat3("Position", &localPositionDummy.x, 0.1f, -90.0f, 90.0f, "%.3f")) {
				SetLocalPosition(localPositionDummy);
			}

			float3 rotDummy = localRotation.ToEulerXYZ();
			rotDummy = RadToDeg(rotDummy);

			if (ImGui::DragFloat3("Rotation", rotDummy.ptr())) {
				Quat localRotationDummy = Quat::FromEulerXYZ(DegToRad(rotDummy.x), DegToRad(rotDummy.y), DegToRad(rotDummy.z));
				SetLocalRotation(localRotationDummy);
			}

			float3 scaleDummy = localScale;
			if (ImGui::DragFloat3("Scale", &scaleDummy.x, 0.1f, -90.0f, 90.0f, "%.3f")) {
				SetLocalScale(scaleDummy);
			}

		}
	}
}

void ComponentTransform::UpdateLocalMatrix() {
	localMatrix = float4x4::FromTRS(localPosition, localRotation, localScale);
}

void ComponentTransform::UpdateGlobalMatrix() {
	globalMatrix = localMatrix;
	if (owner != nullptr) {
		if (owner->parent != nullptr) {
			ComponentTransform* parentTransform = (ComponentTransform*)owner->parent->GetComponentOfType(ComponentType::CTTransform);
			if (parentTransform != nullptr) {
				globalMatrix = parentTransform->globalMatrix * localMatrix;
			}
		}

		for (std::vector<GameObject*>::iterator it = owner->children.begin(); it != owner->children.end(); ++it) {
			((ComponentTransform*)(*it)->GetComponentOfType(ComponentType::CTTransform))->UpdateGlobalValues();
		}
	}
}

void ComponentTransform::DrawGizmos() {
	App->debugDraw->DrawAxisTriad(globalMatrix);
	App->debugDraw->DrawLine(Position(), Position() + Forward() * 4.0f, float3(0.0f, 0, 1.0f));
	App->debugDraw->DrawLine(Position(), Position() + Up() * 4.0f, float3(0, 1.0f, 0));
	App->debugDraw->DrawLine(Position(), Position() + Right() * 4.0f, float3(1.0f, 0, 0.0f));
}

void ComponentTransform::WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	component.AddMember("Component Type", GetType(), alloc);
	component.AddMember("UUID", GetUUID(), alloc);
	component.AddMember("ParentUUID", owner->GetUUID(), alloc);

	rapidjson::Value position(rapidjson::kArrayType);
	position.PushBack(this->localPosition.x, alloc);
	position.PushBack(this->localPosition.y, alloc);
	position.PushBack(this->localPosition.z, alloc);
	component.AddMember("Position", position, alloc);

	rapidjson::Value rotation(rapidjson::kArrayType);
	rotation.PushBack(this->localRotation.x, alloc);
	rotation.PushBack(this->localRotation.y, alloc);
	rotation.PushBack(this->localRotation.z, alloc);
	rotation.PushBack(this->localRotation.w, alloc);
	component.AddMember("Rotation", rotation, alloc);

	rapidjson::Value scale(rapidjson::kArrayType);
	scale.PushBack(this->localScale.x, alloc);
	scale.PushBack(this->localScale.y, alloc);
	scale.PushBack(this->localScale.z, alloc);
	component.AddMember("Scale", scale, alloc);
}


Quat ComponentTransform::CalculateGlobalRotation()const {
	if (owner->parent != nullptr) {
		Component* c = owner->parent->GetComponentOfType(ComponentType::CTTransform);
		if (c != nullptr) {
			return (localRotation * ((ComponentTransform*)c)->CalculateGlobalRotation());
		}
	}
	return localRotation;
}

float3 ComponentTransform::CalculateGlobalScale()const {

	if (owner->parent != nullptr) {
		Component* c = owner->parent->GetComponentOfType(ComponentType::CTTransform);
		if (c != nullptr) {
			return localScale.Mul(((ComponentTransform*)c)->CalculateGlobalScale());
		}
	}
	return localScale;
}

float3 ComponentTransform::CalculateGlobalPosition()const {

	if (owner->parent != nullptr) {
		Component* c = owner->parent->GetComponentOfType(ComponentType::CTTransform);
		if (c != nullptr) {
			return ((ComponentTransform*)c)->CalculateGlobalPosition() + localPosition;
		}
	}

	return localPosition;
}

void ComponentTransform::Reset() {
	localPosition = float3::zero;
	localRotation = Quat::identity;
	localScale = float3::one;
}

void ComponentTransform::OnNewParent(GameObject* oldParent, GameObject* newParent) {
	//if (oldParent == nullptr)return;

	if (newParent == nullptr)return;

	ComponentTransform* newParentTransform = (ComponentTransform*)newParent->GetComponentOfType(ComponentType::CTTransform);
	if (newParentTransform) {
		float4x4 newParentGlobal = newParentTransform->globalMatrix;
		newParentGlobal.Inverse();
		localMatrix = newParentGlobal * globalMatrix;
		localMatrix.Decompose(localPosition, localRotation, localScale);
	}
}


void ComponentTransform::SetPosition(float3 newGlobalPos) {
	float3 globalPos = CalculateGlobalPosition();
	float3 globalMovement = newGlobalPos - globalPos;

	localPosition += globalMovement;

}