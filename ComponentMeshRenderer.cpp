#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include <imgui.h>>
#include "assimp/scene.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Application.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "ModuleDebugDraw.h"
#include "ComponentPointLight.h"
#include "ModuleScene.h"
#include "Leaks.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject* parent) : Component(ComponentType::CTMeshRenderer, parent) {
}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	RELEASE(mesh);
	RELEASE(material);
}


void ComponentMeshRenderer::GenerateAABB() {
	localOrientedBoundingBox = mesh->GetAABB().ToOBB();
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	localOrientedBoundingBox.Transform(transform->globalMatrix);
	localAxisAlignedBoundingBox = localOrientedBoundingBox.MinimalEnclosingAABB();
	owner->GenerateAABB();
}

void ComponentMeshRenderer::Enable() {
	active = true;
}

void ComponentMeshRenderer::Update() {
	Draw();
}

void ComponentMeshRenderer::Disable() {
	active = false;
}

void ComponentMeshRenderer::Draw() {
	mesh->Draw(material, ((ComponentTransform*)owner->GetComponentOfType(CTTransform))->globalMatrix, App->scene->dirLight, App->scene->pointLight);
}

void ComponentMeshRenderer::SetMaterial(Material* mat) {
	material = mat;
}

void ComponentMeshRenderer::DrawOnEditor() {
	//float3(1.0, 0.71, 0.29);
	if (ImGui::CollapsingHeader("Material Values")) {
		float shininessDummy = material->GetShininess();
		if (ImGui::DragFloat("Shininess", &shininessDummy)) {
			material->SetShininess(shininessDummy);
		}

		float3 specularDummy = material->GetSpecularColor();
		if (ImGui::InputFloat3("Specular Color", specularDummy.ptr())) {
			material->SetSpecularColor(specularDummy);
		}
	}
}

void ComponentMeshRenderer::OnTransformChanged() {
	GenerateAABB();
}

void ComponentMeshRenderer::WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	component.AddMember("Component Type", GetType(), alloc);
	component.AddMember("UUID", GetUUID(), alloc);
	component.AddMember("ParentUUID", owner->GetUUID(), alloc);
	component.AddMember("Mesh File", mesh->GetFileID(), alloc);
	component.AddMember("Material File", (rapidjson::Value)rapidjson::StringRef(material->name.c_str()), alloc);
}

void ComponentMeshRenderer::DrawGizmos() {
	if (App->debugDraw) {
		App->debugDraw->DrawAABB(localAxisAlignedBoundingBox);
	}
}

