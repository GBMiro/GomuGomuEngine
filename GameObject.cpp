#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "ComponentPointLight.h"
#include "ComponentDirectionalLight.h"
#include "ComponentSpotLight.h"
#include "ComponentCamera.h"
#include "assimp/scene.h"
#include "Mesh.h"
#include "Globals.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Application.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "ModuleDebugDraw.h"
#include "ModuleScene.h"
#include "Leaks.h"

GameObject::GameObject(GameObject* parent, const char* name) {
	if (parent) {
		this->parent = parent;
		parent->children.push_back(this);
	}
	this->name = name;
	UUID = LCG().Int();
	ComponentTransform* cTransform = new ComponentTransform(this, float3::zero, Quat::identity, float3::one);
	components.push_back(cTransform);
}

GameObject::GameObject(GameObject* parent, const char* name, const float3& position, const Quat& rotation, const float3& scale) {
	if (parent) {
		this->parent = parent;
		parent->children.push_back(this);
	}
	UUID = LCG().Int();
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
	if (Active()) {
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
			(*it)->Update();
		}
	}
}

Component* GameObject::CreateComponent(ComponentType type, int additionalParam) {
	Component* ret = nullptr;
	switch (type) {
	case ComponentType::CTMeshRenderer:
		ret = new ComponentMeshRenderer(this);
		break;
	case ComponentType::CTTransform:
		ret = new ComponentTransform(this, float3::zero, Quat::identity, float3::one);
		break;
	case ComponentType::CTCamera:
		ret = new ComponentCamera(this, 0.1f, 200.0f);
		break;
	case ComponentType::CTLight:

		switch ((ComponentLight::LightType)additionalParam) {
		case ComponentLight::LightType::DIRECTIONAL:
			ret = new ComponentDirectionalLight(this, -float3::unitY);
			break;
		case ComponentLight::LightType::POINT:
			ret = new ComponentPointLight(this);
			break;
		case ComponentLight::LightType::SPOT:
			//TO DO 
			ret = new ComponentSpotLight(this);
			break;
		}


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
	if (meshRenderers.size() > 0) {
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
	} else {

		globalAABB.minPoint = float3::zero;
		globalAABB.maxPoint = float3::zero;
	}

	if (parent != nullptr) {
		parent->GenerateAABB();
	}

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
			retComp = (*it)->GetComponentInChildrenOfType(type);
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

	if (App) {
		if (App->scene) {
			App->scene->ReestablishGameObjectOnQuadTree(this);
		}
	}

}

bool GameObject::Active() const {
	if (parent == nullptr) {
		return active;
	}

	if (parent->Active()) {
		return active;
	}
	return false;
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
	if (!Active())return;
	App->debugDraw->DrawAABB(globalAABB);
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it) {
		(*it)->DrawGizmos();
	}
}

void GameObject::GetAllChilds(std::vector<GameObject*>& children) const {
	for (std::vector<GameObject*>::const_iterator it = this->children.begin(); it != this->children.end(); ++it) {
		children.push_back(*it);
		(*it)->GetAllChilds(children);
	}
}

void GameObject::RemoveParticularComponent(Component* c) {
	std::vector<Component*>::iterator toErase;
	bool found = false;
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end() && !found; ++it) {
		if (*it == c) {
			toErase = it;
			found = true;
		}
	}
	if (found)
		components.erase(toErase);
}


void GameObject::WriteToJSON(rapidjson::Value& gameObject, rapidjson::Document::AllocatorType& alloc) {
	gameObject.AddMember("UUID", GetUUID(), alloc);
	gameObject.AddMember("Parent UUID", parent->GetUUID(), alloc);
	gameObject.AddMember("Name", (rapidjson::Value)rapidjson::StringRef(name.c_str()), alloc);
	if (components.size() > 0) {
		rapidjson::Value gameObjectComponents(rapidjson::kArrayType);
		{
			for (std::vector<Component*>::const_iterator c = components.begin(); c != components.end(); ++c) {
				rapidjson::Value component(rapidjson::kObjectType);
				(*c)->WriteToJSON(component, alloc);
				gameObjectComponents.PushBack(component, alloc);
			}
		}
		gameObject.AddMember("Components", gameObjectComponents, alloc);
	}
}

void GameObject::SetActive(bool should) {
	active = should;
	App->scene->ReestablishGameObjectOnQuadTree(this);
}

/// <summary>
/// Iterates RenderingComponents and calls their abstract override of the Draw method
/// </summary>
void GameObject::Draw() {
	if (!Active())return;

	for (std::vector<RenderingComponent*>::const_iterator it = renderingComponents.begin(); it != renderingComponents.end(); ++it) {
		(*it)->Draw();
	}
}

void GameObject::AddRenderingComponent(RenderingComponent* c) {
	renderingComponents.push_back(c);
}

void GameObject::RemoveRenderingComponent(RenderingComponent* c) {
	std::vector<RenderingComponent*>::iterator it = std::find(renderingComponents.begin(), renderingComponents.end(), c);

	if ((*it) != nullptr)
		renderingComponents.erase(it);

}