#include "Component.h"

Component::Component(ComponentType type) : type(type) {
}

Component::~Component() {
}

ComponentType Component::GetType() const {
	return type;
}
