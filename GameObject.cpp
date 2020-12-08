#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "Globals.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Leaks.h"

GameObject::GameObject(const char* name) {
	this->name = name;
}

GameObject::~GameObject() {

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it) {
		RELEASE(*it);
	}

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		RELEASE(*it);
	}

	childs.clear();
	components.clear();
}

void GameObject::Update() {
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		(*it)->Update();
	}
}

void GameObject::CleanUp() {

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		//(*it)->CleanUp();
	}
}

Component* GameObject::CreateTransformComponent(const aiNode* node) {
	ComponentTransform* com = new ComponentTransform(TRANSFORM, node);
	com->parent = this;
	return com;
}

Component* GameObject::CreateMeshRendererComponent(const aiMesh* mesh) {
	ComponentMeshRenderer* com = new ComponentMeshRenderer(RENDERER, mesh);
	com->parent = this;
	return com;
}

void GameObject::UpdateGameObjectsTransform(const float4x4& parentTransform) {
	globalTransform = parentTransform * GetTransformationMatrix();
	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it) {
		(*it)->UpdateGameObjectsTransform(globalTransform);
	}
}

const float4x4& GameObject::GetTransformationMatrix() const {
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it) {
		if ((*it)->type == TRANSFORM) return ((ComponentTransform*)(*it))->transform;
	}
}

const char* GameObject::GetName() const {
	return name;
}
