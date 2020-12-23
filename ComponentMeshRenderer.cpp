#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Application.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Leaks.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject* parent) : Component(ComponentType::RENDERER, parent) {

}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	RELEASE(mesh);
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
	mesh->Draw(textureName, ((ComponentTransform*)owner->GetComponentByType(TRANSFORM))->globalTransform);
}


