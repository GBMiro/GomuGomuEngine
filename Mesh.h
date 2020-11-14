#pragma once
#include "assimp/scene.h"
#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void Draw(const std::vector<unsigned>& modelTextures);
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

private:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	int materialIndex;
	int numVertex;
	int numIndices;
};

