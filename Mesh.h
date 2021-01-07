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
	Mesh(const aiMesh* mesh);
	~Mesh();

	void Draw(const Material* material, const float4x4& model, const ComponentDirectionalLight*, const ComponentPointLight* pointLight);
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();
	void CreateAABB(const aiMesh* mesh);

	int getNumVertex() const { return numVertex; }

	int getMaterialIndex() const { return materialIndex; }

	int getNumIndices() const { return numIndices; }

	int GetMaterialIndex() const { return materialIndex; }

	const AABB& GetAABB() const { return axisAlignedBB; }

private:
	AABB axisAlignedBB;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	int materialIndex;
	int numVertex;
	int numIndices;
};

