#pragma once
#include "RenderingComponent.h"
#include <string>
#include "Material.h"
#include "../MathGeoLib/Geometry/AABB.h"
#include "../MathGeoLib/Geometry/OBB.h"


class Mesh;

class ComponentMeshRenderer : public RenderingComponent {

public:
	ComponentMeshRenderer(GameObject* parent);
	~ComponentMeshRenderer();

	void GenerateAABB();
	void Enable() override;
	void OnEnable()override;
	void OnDisable()override;
	void Update() override;
	void Disable() override;

	void Draw() override;
	void DrawOnEditor() override;
	void DrawGizmos() override;
	void OnTransformChanged() override;
	void ExposeTextureInfo(const char* type, Material::Texture* tex);
	void ChangeMaterialTexture(Material::Texture* tex, const std::string& textureName);
	void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;
	void LoadFromJSON(const rapidjson::Value& component) override;
	const AABB& GetAABB();
public:
	Mesh* mesh = nullptr;

public:
	void SetMaterial(Material* mat);

private:
	void CreateTexture(TextureType type);
	void ShowTextureIcon(Material::Texture* tex);
private:
	AABB localAxisAlignedBoundingBox;
	OBB localOrientedBoundingBox;
	Material* material = nullptr;

};

