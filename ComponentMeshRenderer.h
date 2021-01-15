#pragma once
#include "RenderingComponent.h"
#include <string>
#include "../MathGeoLib/MathGeoLib.h"

class Mesh;
class Material;

class ComponentMeshRenderer : public RenderingComponent {

public:
	ComponentMeshRenderer(GameObject* parent);
	~ComponentMeshRenderer();

	void GenerateAABB();
	void Enable() override;
	void Update() override;
	void Disable() override;

	void Draw() override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void OnTransformChanged() override;
	//void SetTextureName(std::string name) { textureName = name; }
	void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;
	const AABB& GetAABB();
public:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	AABB localAxisAlignedBoundingBox;
	OBB localOrientedBoundingBox;
public:
	void SetMaterial(Material* mat);

};

