#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "Globals.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Leaks.h"

GameObject::GameObject(GameObject* parent, const char* name) {
	if (parent) {
		this->parent = parent;
		parent->childs.push_back(this);
	}
	this->name = name;
	ComponentTransform* cTransform = new ComponentTransform(this, float3::zero, Quat::identity, float3::one);
	components.push_back(cTransform);
}

GameObject::GameObject(GameObject* parent, const char* name, const float3& position, const Quat& rotation, const float3& scale) {
	if (parent) {
		this->parent = parent;
		parent->childs.push_back(this);
	}
	this->name = name;
	ComponentTransform* cTransform = new ComponentTransform(this, position, rotation, scale);
	components.push_back(cTransform);
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

Component* GameObject::CreateComponent(ComponentType type) {
	Component* ret = nullptr;
	switch (type) {
		case ComponentType::RENDERER:
			ret = new ComponentMeshRenderer(this);
			break;
		case ComponentType::TRANSFORM:
			//ret = new ComponentTransform();
			break;
		case ComponentType::CAMERA:
			break;
	}
	return ret;
}

void GameObject::UpdateGameObjectsTransform(const float4x4& parentTransform) {
	globalTransform = parentTransform * GetTransformationMatrix();
	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it) {
		(*it)->UpdateGameObjectsTransform(globalTransform);
	}
}

void GameObject::ChangeParent(GameObject* newParent) {
	if (newParent != parent) {
		this->parent->childs.erase(std::remove(this->parent->childs.begin(), this->parent->childs.end(), this), this->parent->childs.end());
		parent = newParent;
		newParent->childs.push_back(this);
		LOG("%s's new parent: %s", GetName(), newParent->GetName());
	}
	else LOG("Same parent");
}

bool GameObject::IsAChild(const GameObject* gameObject) const {
	bool found = false;
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end() && !found; ++it) {
		if ((*it) == gameObject) found = true;
		else {
			if((*it)->IsAChild(gameObject)) found = true;
		}
	}
	if (gameObject == this) found = true;
	return found;
}

const float4x4& GameObject::GetTransformationMatrix() const {
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it) {
		if ((*it)->type == TRANSFORM) return ((ComponentTransform*)(*it))->transform;
	}
}

const char* GameObject::GetName() const {
	return name.c_str();
}
