#include "Component.h"

Component::Component(ComponentType type, GameObject* parent) : type(type), owner(parent) {
}

Component::~Component() {
}

ComponentType Component::GetType() const {
	return type;
}
