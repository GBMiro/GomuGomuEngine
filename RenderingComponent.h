#ifndef RENDERING_COMPONENT_H
#define RENDERING_COMPONENT_H
#include "Component.h"
#include <vector>

class ComponentPointLight;
class ComponentDirectionalLight;


class RenderingComponent : public Component {
public:

	std::vector<ComponentPointLight*>closestPointLights;
	ComponentDirectionalLight* directionalLight;

	RenderingComponent(ComponentType type, GameObject* parent);
	~RenderingComponent();


	void CalculateClosestLights();
	virtual void Draw() = 0;
};

#endif // !RENDERING_COMPONENT_H
