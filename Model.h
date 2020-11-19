#pragma once
#include "Module.h"
#include "Mesh.h"
#include "Globals.h"
#include "assimp/scene.h"
#include <vector>

class Model : public Module
{
public:
	Model();
	~Model();

	void Load(const char* filename);
	void LoadMaterials(const aiScene* scene);
	void LoadMeshes(const aiScene* mesh);
	void Draw();

private:
	std::vector<unsigned> materials;
	std::vector<Mesh> meshes; //Change to pointer
};

