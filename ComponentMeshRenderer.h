#pragma once
#include "Component.h"
#include <string>
#include "../MathGeoLib/MathGeoLib.h"

class Mesh;
class Material;

class ComponentMeshRenderer : public Component {

public:
	ComponentMeshRenderer(GameObject* parent);
	~ComponentMeshRenderer();

	void GenerateAABB();
	void Enable()override;
	void Update()override;
	void Disable()override;

	void Draw();
	void DrawOnEditor() override;
	void DrawGizmos() override;
	//void SetTextureName(std::string name) { textureName = name; }

public:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	AABB localAxisAlignedBoundingBox;
	OBB localOrientedBoundingBox;
public:
	void SetMaterial(Material* mat);

};

