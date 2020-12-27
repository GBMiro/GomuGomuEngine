#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Application.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Leaks.h"
#include "Material.h"
#include "ModuleDebugDraw.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject* parent) : Component(ComponentType::CTMeshRenderer, parent) {

}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	RELEASE(mesh);
	RELEASE(material);
}


void ComponentMeshRenderer::GenerateAABB() {
	localOrientedBoundingBox = mesh->GetAABB().ToOBB();
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	//localOrientedBoundingBox.Transform(transform->GetWorldMatrix());
	localAxisAlignedBoundingBox = localOrientedBoundingBox.MinimalEnclosingAABB();
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
	mesh->Draw(material, ((ComponentTransform*)owner->GetComponentOfType(CTTransform))->globalTransform);
}

void ComponentMeshRenderer::SetMaterial(Material* mat) {
	material = mat;
}

void ComponentMeshRenderer::DrawOnEditor() {


}

void ComponentMeshRenderer::DrawGizmos() {
	if (App->debugDraw) {
		App->debugDraw->DrawAABB(localAxisAlignedBoundingBox);
	}
}

