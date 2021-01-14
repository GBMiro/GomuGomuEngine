#include "Component.h"
#include "GameObject.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"
Component::Component(ComponentType type, GameObject* parent) : type(type), owner(parent), enabled(true) {
	UUID = LCG().Int();
}

Component::~Component() {
}

ComponentType Component::GetType() const {
	return type;
}

void Component::Enable() {
	enabled = true;
}

void Component::Disable() {
	enabled = false;
}


bool Component::Enabled()const {
	if (owner->Active()) {
		return enabled;
	}
	return false;
}