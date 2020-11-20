#include "Model.h"
#include "Mesh.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "MathGeoLib/Math/float2.h"
#include "Leaks.h"

Model::Model() {

}

Model::~Model() {
}

void Model::Load(const char* filename)
{
	const aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene) {
		// TODO: LoadTextures(scene->mMaterials, scene->mNumMaterials);
		LoadMaterials(scene);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		LoadMeshes(scene);
	}
	else {
		LOG("Error loading %s: %s", filename, aiGetErrorString());
	}
}

void Model::LoadMaterials(const aiScene* scene)
{
	aiString textureFileName;
	materials.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == AI_SUCCESS) {
			materials.push_back(App->textures->loadTexture(textureFileName.data));
		}
	}
}

void Model::LoadMeshes(const aiScene* scene)
{
	meshes.reserve(scene->mNumMeshes);
	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* currentMesh = scene->mMeshes[i];
		Mesh* mesh = new Mesh();
		mesh->LoadVBO(currentMesh);
		mesh->LoadEBO(currentMesh);
		mesh->CreateVAO();
		meshes.push_back(mesh);
	}
}

void Model::Draw()  {
	for (unsigned i = 0; i < meshes.size(); ++i) {
		meshes[i]->Draw(materials);
	}
}

bool Model::CleanUp() {
	for (unsigned i = 0; i < meshes.size(); ++i) {
		delete (meshes[i]);
	}
	meshes.clear();
	materials.clear();
	return true;
}
