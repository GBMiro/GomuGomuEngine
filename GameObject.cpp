#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "Mesh.h"
#include "Globals.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Leaks.h"

GameObject::GameObject(GameObject* parent, const char* name) {
	if (parent) {
		this->parent = parent;
		parent->children.push_back(this);
	}
	this->name = name;
	ComponentTransform* cTransform = new ComponentTransform(this, float3::zero, Quat::identity, float3::one);
	components.push_back(cTransform);
}

GameObject::GameObject(GameObject* parent, const char* name, const float3& position, const Quat& rotation, const float3& scale) {
	if (parent) {
		this->parent = parent;
		parent->children.push_back(this);
	}
	this->name = name;
	ComponentTransform* cTransform = new ComponentTransform(this, position, rotation, scale);
	components.push_back(cTransform);
}

GameObject::~GameObject() {

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		RELEASE(*it);
	}

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		RELEASE(*it);
	}

	children.clear();
	components.clear();
}

void GameObject::Update() {
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		(*it)->Update();
	}
}

Component* GameObject::CreateComponent(ComponentType type) {
	Component* ret = nullptr;
	switch (type) {
	case ComponentType::CTMeshRenderer:
		ret = new ComponentMeshRenderer(this);
		break;
	case ComponentType::CTTransform:
		//ret = new ComponentTransform();
		break;
	case ComponentType::CTCamera:
		break;
	}

	if (ret != nullptr) {
		components.push_back(ret);
	}

	return ret;

}


void GameObject::ChangeParent(GameObject* newParent) {
	if (newParent != nullptr) {
		if (newParent != parent) {
			RemoveFromParent();//this->parent->children.erase(std::remove(this->parent->children.begin(), this->parent->children.end(), this), this->parent->children.end());
			parent = newParent;
			newParent->children.push_back(this);
			ComponentTransform* cTransform = (ComponentTransform*)GetComponentOfType(CTTransform);
			//TODO: fix transform after changing parent
			cTransform->SetGlobalTransform();
			LOG("%s's new parent: %s", GetName(), newParent->GetName());
		}
		else LOG("Same parent");
	}
}

bool GameObject::IsAChild(const GameObject* gameObject) const {
	bool found = false;
	for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end() && !found; ++it) {
		if ((*it) == gameObject) found = true;
		else {
			if ((*it)->IsAChild(gameObject)) found = true;
		}
	}
	if (gameObject == this) found = true; //This should be remove
	return found;
}

const char* GameObject::GetName() const {
	return name.c_str();
}

void GameObject::GenerateAABB() {

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->GenerateAABB();
	}

	std::vector<AABB> aabb;
	GetChildsAABB(aabb);

	float xMax, xMin, yMax, yMin, zMax, zMin;
	xMax = xMin = yMax = yMin = zMax = zMin = 0;
	if (aabb.size() > 0) {
		xMax = aabb[0].MaxX();
		xMin = aabb[0].MinX();
		yMax = aabb[0].MaxY();
		yMin = aabb[0].MinY();
		zMax = aabb[0].MaxZ();
		zMin = aabb[0].MinZ();
		for (unsigned int i = 1; i < aabb.size(); ++i) {
			if (xMax < aabb[i].MaxX()) xMax = aabb[i].MaxX();
			if (xMin > aabb[i].MinX()) xMin = aabb[i].MinX();
			if (yMax < aabb[i].MaxY()) yMax = aabb[i].MaxY();
			if (yMin > aabb[i].MinY()) yMin = aabb[i].MinY();
			if (zMax < aabb[i].MaxZ()) zMax = aabb[i].MaxZ();
			if (zMin > aabb[i].MinZ()) zMin = aabb[i].MinZ();
		}
		globalAABB.minPoint = vec(xMin, yMin, zMin);
		globalAABB.maxPoint = vec(xMax, yMax, zMax);
	} else {
		globalAABB.minPoint = vec(0, 0, 0);
		globalAABB.maxPoint = vec(0, 0, 0);
	}
}

const AABB& GameObject::GetAABB() const {
	return globalAABB;
}

void GameObject::GetChildsAABB(std::vector<AABB>& aabb) const {
	for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->GetChildsAABB(aabb);
	}
	ComponentMeshRenderer* cRenderer = (ComponentMeshRenderer*)GetComponentOfType(ComponentType::CTMeshRenderer);
	if (cRenderer) {
		//aabb.push_back(cRenderer->mesh->GetAABB()); //Agafar la de cRenderer i no la de cRenderer->mesh
		cRenderer->GenerateAABB();
		aabb.push_back(cRenderer->localAxisAlignedBoundingBox);
	}
}

Component* GameObject::GetComponentOfType(ComponentType type) const {
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it) {
		if ((*it)->GetType() == type) return (*it);
	}
	return nullptr;
}

void GameObject::GetComponentsInChildrenOfType(ComponentType type, std::vector<Component*>& components) const {

	Component* mySelf = GetComponentOfType(type);
	if (mySelf != nullptr) {
		components.push_back(mySelf);
	}
	for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->GetComponentsInChildrenOfType(type, components);
	}
}

Component* GameObject::GetComponentInChildrenOfType(ComponentType type) {
	Component* retComp = nullptr;

	if (components.size() > 0)
		retComp = GetComponentOfType(type);

	if (retComp == nullptr && children.size() > 0) {
		for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end() && retComp == nullptr; ++it) {
			retComp = GetComponentInChildrenOfType(type);
		}
	}
	return retComp;
}

void GameObject::OnTransformChanged() {

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		(*it)->OnTransformChanged();
	}

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->OnTransformChanged();
	}

	GenerateAABB();

}

void GameObject::RemoveFromParent() {
	std::vector<GameObject*>::iterator myItAtParent;
	bool found = false;
	for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end() && !found; ++it) {
		if (*it == this) {
			myItAtParent = it;
			found = true;
		}
	}

	if (parent != nullptr) {
		parent->children.erase(myItAtParent);
	}
}

void GameObject::DrawGizmos() {
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it) {
		(*it)->DrawGizmos();
	}
}
