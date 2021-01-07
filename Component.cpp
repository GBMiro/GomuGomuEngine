#include "Component.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"

Component::Component(ComponentType type, GameObject* parent) : type(type), owner(parent) {
	UUID = LCG().Int();
}

Component::~Component() {
}

ComponentType Component::GetType() const {
	return type;
}
