#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "assimp/scene.h"
#include "Mesh.h"
#include "Globals.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Leaks.h"
#include "Application.h"
#include "ModuleDebugDraw.h"
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

			GameObject* oldParent = parent;

			RemoveFromParent();

			//We substitute the old parent pointer with the new one's

			parent = newParent;
			newParent->children.push_back(this);


			for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
				(*it)->OnNewParent(oldParent, newParent);
			}



			LOG("%s's new parent: %s", GetName(), newParent->GetName());
		} else LOG("Same parent");
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
	float3 absoluteMax = math::vec(-10000, -10000, -10000);
	float3 absoluteMin = float3(100000, 100000, 10000);

	std::vector<Component*>meshRenderers;

	GetComponentsInChildrenOfType(ComponentType::CTMeshRenderer, meshRenderers);

	for (std::vector<Component*>::const_iterator it = meshRenderers.begin(); it != meshRenderers.end(); ++it) {
		AABB meshAABB = ((ComponentMeshRenderer*)(*it))->localAxisAlignedBoundingBox;

		if (meshAABB.minPoint.x < absoluteMin.x) {
			absoluteMin.x = meshAABB.minPoint.x;
		}
		if (meshAABB.minPoint.y < absoluteMin.y) {
			absoluteMin.y = meshAABB.minPoint.y;
		}
		if (meshAABB.minPoint.z < absoluteMin.z) {
			absoluteMin.z = meshAABB.minPoint.z;
		}

		if (meshAABB.maxPoint.x > absoluteMax.x) {
			absoluteMax.x = meshAABB.maxPoint.x;
		}
		if (meshAABB.maxPoint.y > absoluteMax.y) {
			absoluteMax.y = meshAABB.maxPoint.y;
		}
		if (meshAABB.maxPoint.z > absoluteMax.z) {
			absoluteMax.z = meshAABB.maxPoint.z;
		}
	}

	globalAABB.minPoint = absoluteMin;
	globalAABB.maxPoint = absoluteMax;

}

const AABB& GameObject::GetAABB() const {
	return globalAABB;
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

	App->debugDraw->DrawAABB(globalAABB);
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it) {
		(*it)->DrawGizmos();
	}
}
