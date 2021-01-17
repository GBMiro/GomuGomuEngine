#pragma once
#include "assimp/scene.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Geometry/AABB.h"
#include <string.h>
#include <vector>

class ComponentPointLight;
class ComponentDirectionalLight;
class Material;
class Mesh {

public:
	Mesh();
	~Mesh();

	void Load();

	void Draw(const Material* material, const float4x4& model, const ComponentDirectionalLight* dirLight, const std::vector<ComponentPointLight* >& pointLight);
	void CreateAABB();


	int getNumVertex() const { return numVertex; }

	int getMaterialIndex() const { return materialIndex; }

	int getNumIndices() const { return numIndices; }

	int GetMaterialIndex() const { return materialIndex; }

	const AABB& GetAABB() const { return axisAlignedBB; }

	void SetFileID(size_t file) { fileID = file; }
	size_t GetFileID() const { return fileID; }
	const std::vector<Triangle>& GetTransformedTriangles()const;
	const std::vector<Triangle>& GetTriangles()const;
	void CalculateScaledTriangles(const float4x4& modelMat, std::vector<Triangle>& tris);

public:
	int materialIndex;
	int numVertex;
	int numIndices;

	float* vertices = nullptr;
	float* textureCoords = nullptr;
	float* normals = nullptr;
	unsigned* indices = nullptr;
	std::vector<Triangle>unscaledTriangles;
	std::vector<Triangle>scaledTriangles;
private:
	AABB axisAlignedBB;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	size_t fileID;

	void CalculateTriangles(std::vector<Triangle>& tris);

};

