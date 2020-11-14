#pragma once
#include "Module.h"
#include "Globals.h"
#include "assimp/scene.h"
class Model : public Module
{
	Model();
	~Model();

	void Load(const char* filename);
	void LoadMaterials(const aiScene* scene);
};

