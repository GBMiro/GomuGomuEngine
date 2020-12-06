#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "Globals.h"
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
	ComponentTransform* com = new ComponentTransform(TRANSFORM);
	com->parent = this;
	return com;
}

Component* GameObject::CreateMeshRendererComponent(const aiMesh* mesh) {
	ComponentMeshRenderer* com = new ComponentMeshRenderer(RENDERER, mesh);
	com->parent = this;
	return com;
}

const char* GameObject::GetName() const {
	return name;
}
