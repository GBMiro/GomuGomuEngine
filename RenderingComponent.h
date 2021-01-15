#ifndef RENDERING_COMPONENT_H
#define RENDERING_COMPONENT_H
#include "Component.h"
class RenderingComponent : public Component {
public:
	RenderingComponent(ComponentType type, GameObject* parent);
	~RenderingComponent();
	virtual void Draw() = 0;
};

#endif // !RENDERING_COMPONENT_H
