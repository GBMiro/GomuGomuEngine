#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui.h"
#include "Leaks.h"


ComponentTransform::ComponentTransform(GameObject* parent, const float3& position, const Quat& rotation, const float3& scaling) : Component(ComponentType::CTTransform, parent) {

	this->localPosition = position;
	this->localRotation = rotation;
	this->localScale = scaling;

	UpdateLocalMatrix();
	UpdateGlobalMatrix();
}

ComponentTransform::~ComponentTransform() {
}

void ComponentTransform::Enable() {
	active = true;
}

void ComponentTransform::Update() {
	if (!enabled) return;

	if (
		localPosition.x != oldLocalPosition.x || localPosition.y != oldLocalPosition.y || localPosition.z != oldLocalPosition.z ||
		localScale.x != oldLocalScale.x || localScale.y != oldLocalScale.y || localScale.z != oldLocalScale.z ||
		localRotation.x != oldLocalRotation.x || localRotation.z != oldLocalRotation.z || localRotation.y != oldLocalRotation.y || localRotation.w != oldLocalRotation.w
		) {
		UpdateLocalMatrix();
		UpdateGlobalMatrix();
		owner->OnTransformChanged();
	}
}

void ComponentTransform::Disable() {
	active = false;
}

void ComponentTransform::DrawOnEditor() {
	if (owner->parent) {
		if (ImGui::CollapsingHeader("Transformations")) {
			if (ImGui::DragFloat3("Position", &localPosition.x, 0.1f, -90.0f, 90.0f, "%.3f"));

			float3 rotDummy = localRotation.ToEulerXYZ();
			rotDummy = RadToDeg(rotDummy);

			ImGui::DragFloat3("Rotation", rotDummy.ptr());

			if (oldRotDummy.x != rotDummy.x || oldRotDummy.y != rotDummy.y || oldRotDummy.z != rotDummy.z) {

				localRotation = Quat::FromEulerXYZ(DegToRad(rotDummy.x), DegToRad(rotDummy.y), DegToRad(rotDummy.z));
				oldRotDummy = rotDummy;
			}

			if (ImGui::DragFloat3("Scale", &localScale.x, 0.1f, -90.0f, 90.0f, "%.3f"));
		}
	}
}

void ComponentTransform::UpdateLocalMatrix() {
	localMatrix = float4x4::FromTRS(localPosition, localRotation, localScale);
	oldLocalPosition = localPosition;
	oldLocalScale = localScale;
	oldLocalRotation = localRotation;
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
			((ComponentTransform*)(*it)->GetComponentOfType(ComponentType::CTTransform))->UpdateGlobalMatrix();
		}
	}
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
	if (oldParent == nullptr)return;
	ComponentTransform* prevParentTransform = (ComponentTransform*)oldParent->GetComponentOfType(ComponentType::CTTransform);
	ComponentTransform* newParentTransform = (ComponentTransform*)newParent->GetComponentOfType(ComponentType::CTTransform);
	if (prevParentTransform != nullptr) {

		//To keep the local values untouched, we add the previous parent's global position to the localPosition and remove the current's global position from the localPosition
		float3 prevGlobalPos = prevParentTransform->CalculateGlobalPosition() + localPosition;
		float3 globalPos = CalculateGlobalPosition() - localPosition;
		localPosition = prevGlobalPos - globalPos;

		//Same for scale
		float3 prevGlobalScale = prevParentTransform->CalculateGlobalScale().Mul(localScale);
		float3 globalScale = CalculateGlobalScale().Div(localScale);
		localScale = prevGlobalScale.Div(globalScale);

		//Same for rotation
		Quat prevGlobalRot = prevParentTransform->CalculateGlobalRotation();
		Quat newGlobalRot = newParentTransform->CalculateGlobalRotation();

		//We inverse so that it is subtracted and not added (multiplication order may need to be switched)
		newGlobalRot.Inverse();
		localRotation = newGlobalRot * prevGlobalRot * localRotation;

	}
}
