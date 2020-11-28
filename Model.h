#pragma once
#include "Module.h"
#include "Mesh.h"
#include "Globals.h"
#include "assimp/scene.h"
#include "MathGeoLib/Math/float3.h"
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

	void setMinFilter(unsigned index) const;
	void setMagFilter(unsigned index) const;
	void setWrapMode(unsigned index) const;

	void processFile(const char* filename);

	void deleteMeshes();
	void deleteTextures();

private:
	void calculateModelCenter();
	void loadNewTexture(const char* textureName, const char* filename);

public:
	float3 modelCenter;
	float sphereRadius;

private:
	std::vector<unsigned> materials;
	std::vector<Mesh*> meshes;
};

inline void assimpToLOG(const char* msg, char* userData) {
	LOG(msg);
}
