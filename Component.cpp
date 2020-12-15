#include "Component.h"

Component::Component(ComponentType type, GameObject* parent) : type(type), parent(parent) {
}

Component::~Component() {
}

ComponentType Component::GetType() const {
	return type;
}
