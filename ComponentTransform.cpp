#include "ComponentTransform.h"
#include "Leaks.h"

ComponentTransform::ComponentTransform(ComponentType type) : Component(type) {
}

ComponentTransform::~ComponentTransform() {
}

void ComponentTransform::Enable() {
	active = true;
}

void ComponentTransform::Update() {
}

void ComponentTransform::Disable() {
	active = false;
}
