#pragma once
#include "RenderingComponent.h"
#include <string>
#include "Material.h" // Talk with David enum
#include "../MathGeoLib/MathGeoLib.h"

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
	void ShowTextureInfo(const char* type, Material::Texture* tex);
	void ChangeMaterialTexture(Material::Texture* tex, std::string& textureName);
	//void SetTextureName(std::string name) { textureName = name; }
	void WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) override;
	void LoadFromJSON(const rapidjson::Value& component) override;
	const AABB& GetAABB();
public:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	AABB localAxisAlignedBoundingBox;
	OBB localOrientedBoundingBox;
public:
	void SetMaterial(Material* mat);

private:
	void CreateTexture(TextureType type);
private:

};

