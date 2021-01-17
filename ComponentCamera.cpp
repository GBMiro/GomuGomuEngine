#include "ComponentCamera.h"
#include "imgui.h"
#include "Leaks.h"
#include "Application.h"
#include "ModuleDebugDraw.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"

void ComponentCamera::Enable() {

}

void ComponentCamera::Update() {

}

void ComponentCamera::Disable() {

}

void ComponentCamera::OnEnable() {

}
void ComponentCamera::OnDisable() {

}

void ComponentCamera::DrawOnEditor() {
	if (ImGui::CollapsingHeader("Camera")) {
		bool thisCameraCulls = App->renderer->GetCullingCamera() == this;
		bool frustumCulling = App->renderer->GetFrustumCulling() && thisCameraCulls;

		if (ImGui::Checkbox("Frustum Culling", &frustumCulling)) {
			App->renderer->SetCullingCamera(frustumCulling ? this : nullptr);
		}

	}
}

void ComponentCamera::OnNewParent(GameObject* prevParent, GameObject* newParent) {

}

Frustum& ComponentCamera::GetFrustum() {
	return frustum;
}

ComponentCamera::~ComponentCamera() {

	if (App->renderer != nullptr) {
		if (App->renderer->GetCullingCamera() == this) {
			App->renderer->SetCullingCamera(nullptr);
		}
	}
}
//
void ComponentCamera::SetUpFrustum(float nearDistance, float farDistance) {
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearDistance, farDistance);
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(90), 1.77f);
	frustum.SetPos(transform->localPosition);
	transform->SetLocalRotation(Quat::identity);
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);
}

void ComponentCamera::WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	component.AddMember("Component Type", GetType(), alloc);
	component.AddMember("UUID", GetUUID(), alloc);
	component.AddMember("ParentUUID", owner->GetUUID(), alloc);
	component.AddMember("Enabled", enabled, alloc);
	component.AddMember("Horizontal FOV", RadToDeg(frustum.HorizontalFov()), alloc);
	component.AddMember("Aspect Ratio", frustum.AspectRatio(), alloc);
	component.AddMember("Near Distance", frustum.NearPlaneDistance(), alloc);
	component.AddMember("Far Distance", frustum.FarPlaneDistance(), alloc);
}

void ComponentCamera::LoadFromJSON(const rapidjson::Value& component) {
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(component["Near Distance"].GetFloat(), component["Far Distance"].GetFloat());
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(component["Horizontal FOV"].GetFloat()), component["Aspect Ratio"].GetFloat());
	if (component.HasMember("Enabled")) {
		enabled = component["Enabled"].GetBool();
	}
	else {
		enabled = true;
	}
}


ComponentCamera::ComponentCamera(GameObject* anOwner, float aNearPDistance, float aFarPDistance) : Component(ComponentType::CTCamera, anOwner) {
	SetUpFrustum(aNearPDistance, aFarPDistance);
}


void ComponentCamera::DrawGizmos() {
	if (App->debugDraw != nullptr) {
		App->debugDraw->DrawFrustum(frustum);
	}
}

void ComponentCamera::OnTransformChanged() {

	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);

	frustum.SetPos(transform->Position());
	if (owner->parent != nullptr) {
		frustum.SetFront(transform->Forward());
		frustum.SetUp(transform->Up());
	}
}
