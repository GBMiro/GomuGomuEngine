#pragma once
#include "Module.h"
#include "Mesh.h"
#include "Globals.h"
#include "assimp/scene.h"
#include <vector>

using namespace Assimp;

class Model : public Module
{
public:
	Model();
	~Model();

	void Load(const char* filename);
	void LoadMaterials(const aiScene* scene, const char* filename);
	void LoadMeshes(const aiScene* mesh);
	void Draw();
	
	bool CleanUp();

	void getMeshes(std::vector<Mesh*>& meshes) const;
	void getTextures(std::vector<unsigned>& textures) const;

	void setMinMaxFilter(bool active) const;

private:
	std::vector<unsigned> materials;
	std::vector<Mesh*> meshes; //Change to pointer
};

