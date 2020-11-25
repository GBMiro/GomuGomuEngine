#pragma once
#include "assimp/scene.h"
#include <vector>

class Mesh
{
	struct mAABB {
		float xMin;
		float xMax;
		float yMin;
		float yMax;
		float zMin;
		float zMax;
	};

public:
	Mesh();
	~Mesh();

	void Draw(const std::vector<unsigned>& modelTextures);
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	int getNumVertex() const { return numVertex; }

	int getMaterialIndex() const { return materialIndex; }

	int getNumIndices() const { return numIndices; }

public:
	mAABB boundingBox;

private:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	int materialIndex;
	int numVertex;
	int numIndices;
};

