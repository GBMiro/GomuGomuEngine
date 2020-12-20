#pragma once
#include "assimp/scene.h"
#include "MathGeoLib/Math/float4x4.h"
#include <string.h>
#include <vector>

class Mesh
{

public:
	Mesh(const aiMesh* mesh);
	~Mesh();

	void Draw(const std::string& textureName, const float4x4& model);
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	int getNumVertex() const { return numVertex; }

	int getMaterialIndex() const { return materialIndex; }

	int getNumIndices() const { return numIndices; }

	int GetMaterialIndex() const { return materialIndex; }

private:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	int materialIndex;
	int numVertex;
	int numIndices;
};

