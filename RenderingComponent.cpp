#include "RenderingComponent.h"
#include "ComponentPointLight.h"
#include "ComponentDirectionalLight.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Globals.h"
#include <iostream>
#include <algorithm>
#include "MathGeoLib/Math/float3.h"

RenderingComponent::RenderingComponent(ComponentType type, GameObject* parent) :Component(type, parent), directionalLight(nullptr) {
	closestPointLights.reserve(MAX_POINT_LIGHTS);
	CalculateClosestLights();
}

RenderingComponent::~RenderingComponent() {

}


void RenderingComponent::CalculateClosestLights() {
	closestPointLights.clear();
	directionalLight = nullptr;

	ComponentTransform* objTransform = (ComponentTransform*)(owner->GetComponentOfType(ComponentType::CTTransform));
	if (objTransform == nullptr) return;

	for (std::vector<ComponentLight*>::const_iterator it = App->scene->sceneLights.begin(); it != App->scene->sceneLights.end(); ++it) {
		switch ((*it)->GetLightType()) {
		case ComponentLight::LightType::POINT:

			closestPointLights.push_back((ComponentPointLight*)*it);

			if (closestPointLights.size() > MAX_POINT_LIGHTS) {
				float3 aPos = owner->GetAABB().CenterPoint();

				std::sort(closestPointLights.begin(), closestPointLights.end(), [aPos](ComponentPointLight* l, ComponentPointLight* r) {
					float lDist = (aPos - ((ComponentTransform*)l->owner->GetComponentOfType(ComponentType::CTTransform))->Position()).Length();
					float rDist = (aPos - ((ComponentTransform*)r->owner->GetComponentOfType(ComponentType::CTTransform))->Position()).Length();

					return lDist < rDist;
					}
				);
			}

			break;
		case ComponentLight::LightType::DIRECTIONAL:
			if (directionalLight == nullptr) {
				directionalLight = (ComponentDirectionalLight*)*it;
			}
			break;
		default:
			break;
		}
	}
}